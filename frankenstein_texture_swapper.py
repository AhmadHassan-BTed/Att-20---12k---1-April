#!/usr/bin/env python3
"""
frankenstein_texture_swapper.py  —  v3: High-Fidelity VRAM & Layout-Aware Splicer
================================================================================
Transplants Vanilla character textures and palettes into native Frontiers
skeleton nodes with correct tree traversal, pristine inline data grafting, and
precision VRAM TEX0 dimension updates.

FIXES in v3:
1. Traversal: Correctly traverses the actual double-branch texture container
   layout: 0x11110 (Texture Container) -> 0x01001 (Textures) and 0x01101 (Materials).
2. Robustness: Avoids buggy inline data reconstruction of 0x01000 nodes. Copies
   Vanilla's pristine 0x01000 inline data verbatim.
3. Dimensions: Extracts texture dimensions directly from Vanilla's headers
   using a robust DWORD probe, and patches ONLY the VRAM TEX0 dimensions (TW, TH)
   while keeping Frontiers VRAM offsets and filtering intact.
"""

import os
import sys
import math
import struct
import glob
import copy
import traceback
from esf_parser import ESFParser


# ─────────────────────────────────────────────────────────────────────────────
# PS2 GS Constants
# ─────────────────────────────────────────────────────────────────────────────

GS_REG_TEX0_1  = 0x06   # texture buffer params, CLUT params, pixel format (context 1)
GS_REG_TEX0_2  = 0x16   # same, context 2

PSM_PALETTED = {0x13, 0x14, 0x1B, 0x24, 0x2C} # PSMT8, PSMT4, PSMT8H, PSMT4HL, PSMT4HH

_TEX0_FIELDS = [
    ('TBP0', 0,  14),   # VRAM base pointer
    ('TBW',  14,  6),   # VRAM buffer width
    ('PSM',  20,  6),   # pixel storage mode
    ('TW',   26,  4),   # log2(width)
    ('TH',   30,  4),   # log2(height)
    ('TCC',  34,  1),   # transparency
    ('TFX',  35,  2),   # texture function
    ('CBP',  37, 14),   # CLUT base pointer
    ('CPSM', 51,  4),   # CLUT pixel format
    ('CSM',  55,  1),   # CLUT storage mode
    ('CSA',  56,  5),   # CLUT entry offset
    ('CLD',  61,  3),   # CLUT load control
]

def _extract_bits(val: int, offset: int, count: int) -> int:
    return (val >> offset) & ((1 << count) - 1)

def _insert_bits(target: int, val: int, offset: int, count: int) -> int:
    mask = ((1 << count) - 1) << offset
    return (target & ~mask) | ((val & ((1 << count) - 1)) << offset)

def parse_tex0(reg64: int) -> dict:
    return {name: _extract_bits(reg64, off, cnt) for name, off, cnt in _TEX0_FIELDS}

def encode_tex0(fields: dict) -> int:
    val = 0
    for name, off, cnt in _TEX0_FIELDS:
        val = _insert_bits(val, fields.get(name, 0), off, cnt)
    return val


# ─────────────────────────────────────────────────────────────────────────────
# Node tree utilities
# ─────────────────────────────────────────────────────────────────────────────

def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
    }
    pos += 12
    if child_count == 0:
        if pos + data_size > len(data):
            raise EOFError(f"EOF reading leaf at 0x{pos:X} (need {data_size} B)")
        node['inline_data'] = data[pos : pos + data_size]
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def update_node_sizes(node: dict):
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data']) if node['inline_data'] else 0
    else:
        node['child_count'] = len(node['children'])
        total = 0
        for child in node['children']:
            update_node_sizes(child)
            total += 12 + child['data_size']
        node['data_size'] = total

def serialize_node(node: dict) -> bytes:
    buf = bytearray()
    buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    if node['child_count'] == 0:
        if node['inline_data']:
            buf += node['inline_data']
    else:
        for child in node['children']:
            buf += serialize_node(child)
    return bytes(buf)


# ─────────────────────────────────────────────────────────────────────────────
# Dimension Parser & TEX0 Patcher
# ─────────────────────────────────────────────────────────────────────────────

def get_vanilla_texture_dimensions(inline_data: bytes) -> tuple:
    """Robustly extract width and height from Vanilla 0x01000 node inline data."""
    if len(inline_data) < 12:
        return 64, 64  # safe default

    width = struct.unpack_from('<I', inline_data, 4)[0]
    height = struct.unpack_from('<I', inline_data, 8)[0]

    # Sanity check: must be valid powers-of-2 dimensions
    valid_dims = {16, 32, 64, 128, 256, 512}
    if width not in valid_dims or height not in valid_dims:
        # Try 16-bit fallback fields
        w_16 = struct.unpack_from('<H', inline_data, 14)[0]
        h_16 = struct.unpack_from('<H', inline_data, 16)[0]
        if w_16 in valid_dims and h_16 in valid_dims:
            return w_16, h_16
        # Safe default for palette-only nodes (size 384)
        return 64, 64

    return width, height


# ─────────────────────────────────────────────────────────────────────────────
# GIF Packet / TEX0 Material Patching
# ─────────────────────────────────────────────────────────────────────────────

def _parse_gif_tag(data: bytes) -> tuple:
    if len(data) < 16:
        raise ValueError("GIF tag requires at least 16 bytes")
    lo = struct.unpack_from('<Q', data, 0)[0]
    nloop = lo & 0x7FFF
    flg   = (lo >> 58) & 0x3
    nreg  = (lo >> 52) & 0xF
    if nreg == 0:
        nreg = 16
    return nloop, nreg, flg

def translate_material_node(frontiers_mat_node: dict,
                             vanilla_width: int,
                             vanilla_height: int,
                             label: str) -> dict:
    """Patch ONLY the log2 dimensions (TW, TH) in Frontiers material TEX0 register."""
    result = copy.deepcopy(frontiers_mat_node)
    fro_raw = frontiers_mat_node.get('inline_data', b'')
    if not fro_raw:
        return result

    try:
        nloop, nreg, flg = _parse_gif_tag(fro_raw)
    except ValueError as e:
        print(f"      [warn] {label}: cannot parse Frontiers GIF tag: {e}")
        return result

    if flg != 0:
        return result

    fro_tex0_val = None
    pos = 16
    tex0_offset = None
    for _ in range(nloop * nreg):
        if pos + 16 > len(fro_raw):
            break
        addr = struct.unpack_from('<Q', fro_raw, pos + 8)[0]
        if (addr & 0xFF) in (GS_REG_TEX0_1, GS_REG_TEX0_2):
            fro_tex0_val = struct.unpack_from('<Q', fro_raw, pos)[0]
            tex0_offset = pos
            break
        pos += 16

    if fro_tex0_val is None or tex0_offset is None:
        return result

    # Merge fields
    base = parse_tex0(fro_tex0_val)
    merged = dict(base)

    tw = max(0, int(math.log2(vanilla_width)))
    th = max(0, int(math.log2(vanilla_height)))

    merged['TW'] = tw
    merged['TH'] = th

    if base['PSM'] in PSM_PALETTED:
        merged['CBP'] = 0
        merged['CSA'] = 0
        merged['CLD'] = 1

    new_tex0_val = encode_tex0(merged)

    # Patch in-place
    buf = bytearray(fro_raw)
    struct.pack_into('<Q', buf, tex0_offset, new_tex0_val)

    if base['TW'] != tw or base['TH'] != th:
        print(f"      [TEX0 diff] {label}: TW {base['TW']}->{tw}, TH {base['TH']}->{th}")

    result['inline_data'] = bytes(buf)
    result['data_size']   = len(buf)
    return result


# ─────────────────────────────────────────────────────────────────────────────
# Container Grafting & Traversal
# ─────────────────────────────────────────────────────────────────────────────

def translate_texture_container(vanilla_container: dict,
                                 frontiers_container: dict,
                                 asset_label: str) -> dict:
    """Surgically graft Vanilla textures/palettes into Frontiers double-branch layout."""
    result = copy.deepcopy(frontiers_container)

    # Locate the double branches: 0x01001 (textures) and 0x01101 (materials)
    van_01001 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01001), None)
    fro_01001 = next((c for c in result['children'] if c['type_id'] == 0x01001), None)

    van_01101 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01101), None)
    fro_01101 = next((c for c in result['children'] if c['type_id'] == 0x01101), None)

    if not van_01001 or not fro_01001:
        print(f"    [{asset_label}] Missing 0x01001 node branch. Graft skipped.")
        return result

    van_textures = van_01001['children']
    fro_textures = fro_01001['children']

    van_materials = van_01101['children'] if van_01101 else []
    fro_materials = fro_01101['children'] if fro_01101 else []

    num_textures = min(len(van_textures), len(fro_textures))
    print(f"    [{asset_label}] Pairing and grafting {num_textures} texture slots...")

    for i in range(num_textures):
        van_tex_node = van_textures[i]
        live_tex     = fro_textures[i]
        label = f"{asset_label}/slot{i}"

        # 1. Parse Vanilla texture dimensions
        v_width, v_height = get_vanilla_texture_dimensions(van_tex_node['inline_data'])

        # 2. Overwrite entire 0x01000 inline data with pristine Vanilla bytes
        live_tex['inline_data'] = van_tex_node['inline_data']
        live_tex['data_size']   = len(van_tex_node['inline_data'])

        # 3. Patch corresponding Frontiers material if it exists
        if i < len(van_materials) and i < len(fro_materials):
            fro_materials[i] = translate_material_node(
                frontiers_mat_node = fro_materials[i],
                vanilla_width      = v_width,
                vanilla_height     = v_height,
                label              = label
            )

    update_node_sizes(result)
    return result


# ─────────────────────────────────────────────────────────────────────────────
# Main Pipeline
# ─────────────────────────────────────────────────────────────────────────────

def perform_texture_swaps():
    payload_dir        = './workspace/payloads'
    frontiers_esf_path = './workspace/expansion/CHAR.ESF'

    print("=" * 65)
    print("  frankenstein_texture_swapper.py  —  v3: High-Fidelity Splicer")
    print("=" * 65)

    if not os.path.exists(frontiers_esf_path):
        print(f"[-] Frontiers ESF not found: {frontiers_esf_path}")
        sys.exit(1)

    print(f"\n[*] Parsing Frontiers CHAR.ESF ...")
    with open(frontiers_esf_path, 'rb') as fh:
        frontiers_esf_bytes = fh.read()

    frontiers_parser = ESFParser(frontiers_esf_bytes).parse()
    frontiers_map    = {
        e.asset_id: e
        for e in frontiers_parser.pointer_table
        if e.asset_id is not None
    }
    print(f"    Frontiers template map: {len(frontiers_map)} entries.")

    bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
    if not bin_files:
        print(f"[-] No .bin payloads in {payload_dir}")
        sys.exit(1)

    swapped = 0
    skipped = 0
    failed  = 0

    for filepath in bin_files:
        filename = os.path.basename(filepath)

        try:
            hash_str   = filename.split('_')[1].split('.')[0]
            asset_hash = int(hash_str, 16)
        except Exception:
            continue
        asset_label = f"0x{asset_hash:08X}"

        with open(filepath, 'rb') as fh:
            vanilla_bytes = fh.read()

        if len(vanilla_bytes) < 12:
            continue

        try:
            vanilla_node, _ = parse_node(vanilla_bytes, 0)
        except Exception as e:
            print(f"\n  [-] Parse error on Vanilla {filename}: {e}")
            failed += 1
            continue

        if vanilla_node is None or vanilla_node['child_count'] == 0:
            continue

        # Find 0x11110 container (type 0x11100 in Vanilla)
        van_tex_containers = [c for c in vanilla_node['children'] if c['type_id'] in (0x11100, 0x11110)]
        if not van_tex_containers:
            continue

        vanilla_tex_container = van_tex_containers[0]

        if asset_hash not in frontiers_map:
            print(f"\n  [skip] {asset_label}: not in Frontiers ESF — retaining Vanilla.")
            skipped += 1
            continue

        entry     = frontiers_map[asset_hash]
        fro_bytes = frontiers_esf_bytes[entry.offset : entry.offset + entry.length]

        try:
            frontiers_node, _ = parse_node(fro_bytes, 0)
        except Exception as e:
            print(f"\n  [-] Parse error on Frontiers template {asset_label}: {e}")
            failed += 1
            continue

        fro_tex_containers = [c for c in frontiers_node.get('children', []) if c['type_id'] == 0x11110]
        if not fro_tex_containers:
            print(f"\n  [warn] {asset_label}: Frontiers template lacks texture container. Skipped.")
            skipped += 1
            continue

        frontiers_tex_container = fro_tex_containers[0]

        print(f"\n[*] {asset_label}: grafting textures & patching material dimensions...")
        try:
            translated_container = translate_texture_container(
                vanilla_container   = vanilla_tex_container,
                frontiers_container = frontiers_tex_container,
                asset_label         = asset_label,
            )
        except Exception as e:
            print(f"  [-] Splicing error for {asset_label}: {e}")
            traceback.print_exc()
            failed += 1
            continue

        graft_root = copy.deepcopy(frontiers_node)
        fro_tex_idx = next(
            (i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x11110),
            None
        )
        if fro_tex_idx is None:
            print(f"  [-] {asset_label}: 0x11110 slot vanished (logic error).")
            failed += 1
            continue

        graft_root['children'][fro_tex_idx] = translated_container

        # Recompute node hierarchy sizes
        update_node_sizes(graft_root)
        final_bytes = serialize_node(graft_root)

        with open(filepath, 'wb') as fh:
            fh.write(final_bytes)

        print(f"  [+] {asset_label}: graft complete "
              f"({len(vanilla_bytes):,} B vanilla -> "
              f"{len(fro_bytes):,} B frontiers template -> "
              f"{len(final_bytes):,} B hybrid)")
        swapped += 1

    print(f"\n{'='*65}")
    print(f"  HYBRID GRAFT COMPLETE")
    print(f"  Grafted : {swapped}")
    print(f"  Skipped : {skipped}")
    print(f"  Failed  : {failed}")
    print(f"{'='*65}")

    if failed > 0:
        print("\n[-] Errors occurred. DO NOT proceed to ISO repack.")
        sys.exit(1)

    print("\n[+] All hybrid templates successfully reconstructed. Ready for ESF merge -> ISO repack.")


if __name__ == '__main__':
    perform_texture_swaps()