#!/usr/bin/env python3
"""
deep_surgery_fix.py  —  Master Dual-Surgery Pipeline for EQOA Models
====================================================================
A complete, fully automated master pipeline that:
1. Re-extracts clean, pristine classic character payloads from Vanilla.
2. Splicers and grafts Vanilla textures into Frontiers skeletons, patching VRAM TEX0 dimensions.
3. Hex-splices the Frontiers headers onto Vanilla's 3D mesh geometry packets.
4. Auto-patches the size fields inside the hybrid headers.
5. Rebuilds the model database and repacks the playable ISO.
"""

import os
import sys
import math
import struct
import glob
import copy
import shutil
import subprocess
from esf_parser import ESFParser

# ─────────────────────────────────────────────────────────────────────────────
# Constants for GS / VIF Splicing
# ─────────────────────────────────────────────────────────────────────────────
VIF_UNPACK_MSB_MIN = 0x60
VIF_UNPACK_MSB_MAX = 0x7F
VIF_STCYCL_MSB     = 0x01
CONSECUTIVE_VIF_THRESHOLD = 3
SIZE_FIELD_PROBE_OFFSETS = [0x04, 0x08, 0x0C, 0x10, 0x14]
MIN_PLAUSIBLE_PAYLOAD_SIZE = 64
MAX_PLAUSIBLE_PAYLOAD_SIZE = 8 * 1024 * 1024

GS_REG_TEX0_1  = 0x06
GS_REG_TEX0_2  = 0x16
PSM_PALETTED = {0x13, 0x14, 0x1B, 0x24, 0x2C}

_TEX0_FIELDS = [
    ('TBP0', 0,  14), ('TBW',  14,  6), ('PSM',  20,  6), ('TW',   26,  4),
    ('TH',   30,  4), ('TCC',  34,  1), ('TFX',  35,  2), ('CBP',  37, 14),
    ('CPSM', 51,  4), ('CSM',  55,  1), ('CSA',  56,  5), ('CLD',  61,  3),
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
# Node Parsing Utilities
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
# Mesh scanning and size patching
# ─────────────────────────────────────────────────────────────────────────────
def find_mesh_start(data: bytes, label: str) -> int:
    file_len   = len(data)
    run_start  = -1    
    run_length = 0     

    for pos in range(0, file_len - 4, 4):
        dword = struct.unpack_from('<I', data, pos)[0]
        msb   = (dword >> 24) & 0xFF
        is_vif = (VIF_UNPACK_MSB_MIN <= msb <= VIF_UNPACK_MSB_MAX) or (msb == VIF_STCYCL_MSB)

        if is_vif:
            if run_length == 0:
                run_start = pos          
            run_length += 1
            if run_length >= CONSECUTIVE_VIF_THRESHOLD:
                return run_start
        else:
            run_start  = -1
            run_length = 0
    return -1

def patch_size_fields(header_blob: bytearray, new_total_size: int, original_donor_size: int) -> list:
    patched_offsets = []
    tolerance = 512   
    for off in SIZE_FIELD_PROBE_OFFSETS:
        if off + 4 > len(header_blob):
            continue
        current_val = struct.unpack_from('<I', header_blob, off)[0]
        if not (MIN_PLAUSIBLE_PAYLOAD_SIZE <= current_val <= MAX_PLAUSIBLE_PAYLOAD_SIZE):
            continue
        if abs(int(current_val) - original_donor_size) <= tolerance:
            old_val = current_val
            struct.pack_into('<I', header_blob, off, new_total_size)
            patched_offsets.append(off)
    return patched_offsets

# ─────────────────────────────────────────────────────────────────────────────
# Dimension Parsing & TEX0 Patcher
# ─────────────────────────────────────────────────────────────────────────────
def get_vanilla_texture_dimensions(inline_data: bytes) -> tuple:
    if len(inline_data) < 12:
        return 64, 64
    width = struct.unpack_from('<I', inline_data, 4)[0]
    height = struct.unpack_from('<I', inline_data, 8)[0]
    valid_dims = {16, 32, 64, 128, 256, 512}
    if width not in valid_dims or height not in valid_dims:
        w_16 = struct.unpack_from('<H', inline_data, 14)[0]
        h_16 = struct.unpack_from('<H', inline_data, 16)[0]
        if w_16 in valid_dims and h_16 in valid_dims:
            return w_16, h_16
        return 64, 64
    return width, height

def translate_material_node(frontiers_mat_node: dict, vanilla_width: int, vanilla_height: int) -> dict:
    result = copy.deepcopy(frontiers_mat_node)
    fro_raw = frontiers_mat_node.get('inline_data', b'')
    if not fro_raw:
        return result

    try:
        lo = struct.unpack_from('<Q', fro_raw, 0)[0]
        nloop = lo & 0x7FFF
        flg   = (lo >> 58) & 0x3
        nreg  = (lo >> 52) & 0xF
        if nreg == 0: nreg = 16
    except Exception:
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
    buf = bytearray(fro_raw)
    struct.pack_into('<Q', buf, tex0_offset, new_tex0_val)

    result['inline_data'] = bytes(buf)
    result['data_size']   = len(buf)
    return result

# ─────────────────────────────────────────────────────────────────────────────
# Container Grafting & Traversal
# ─────────────────────────────────────────────────────────────────────────────
def translate_texture_container(vanilla_container: dict, frontiers_container: dict, asset_label: str) -> dict:
    result = copy.deepcopy(frontiers_container)
    van_01001 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01001), None)
    fro_01001 = next((c for c in result['children'] if c['type_id'] == 0x01001), None)
    van_01101 = next((c for c in vanilla_container['children'] if c['type_id'] == 0x01101), None)
    fro_01101 = next((c for c in result['children'] if c['type_id'] == 0x01101), None)

    if not van_01001 or not fro_01001:
        return result

    van_textures = van_01001['children']
    fro_textures = fro_01001['children']
    van_materials = van_01101['children'] if van_01101 else []
    fro_materials = fro_01101['children'] if fro_01101 else []

    num_textures = min(len(van_textures), len(fro_textures))
    for i in range(num_textures):
        van_tex_node = van_textures[i]
        live_tex     = fro_textures[i]
        v_width, v_height = get_vanilla_texture_dimensions(van_tex_node['inline_data'])
        live_tex['inline_data'] = van_tex_node['inline_data']
        live_tex['data_size']   = len(van_tex_node['inline_data'])

        if i < len(van_materials) and i < len(fro_materials):
            fro_materials[i] = translate_material_node(fro_materials[i], v_width, v_height)

    update_node_sizes(result)
    return result

# ─────────────────────────────────────────────────────────────────────────────
# Master Execution Pipeline
# ─────────────────────────────────────────────────────────────────────────────
def main():
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    payloads_dir = 'workspace/payloads'

    print("=" * 70)
    print("  EQOA DUAL-SURGERY MASTER PIPELINE (Textures & Geometry Graft)")
    print("=" * 70)

    # 1. Fresh programmatic extraction of pristine Vanilla payloads
    print("\n[*] Step 1: Performing fresh extraction of pristine Vanilla payloads...")
    from payload_extractor import extract_all_payloads
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    extract_all_payloads(original_esf, payloads_dir)

    # 2. Parse Frontiers templates
    print(f"\n[*] Step 2: Parsing Frontiers templates...")
    with open(expansion_esf, 'rb') as f:
        fro_esf_bytes = f.read()
    fro_parser = ESFParser(fro_esf_bytes).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}
    print(f"    Mapped {len(fro_map)} Frontiers templates.")

    # 3. Perform dual surgery on all payloads
    bin_files = sorted(glob.glob(os.path.join(payloads_dir, '*.bin')))
    print(f"\n[*] Step 3: Executing dual-surgery on {len(bin_files)} payloads...")

    processed = 0
    spliced = 0
    pure_headers = 0

    for filepath in bin_files:
        filename = os.path.basename(filepath)
        try:
            hash_str = filename.split('_')[1].split('.')[0]
            asset_hash = int(hash_str, 16)
        except Exception:
            continue
        asset_label = f"0x{asset_hash:08X}"

        # Load Vanilla payload
        with open(filepath, 'rb') as f:
            van_bytes = f.read()

        if len(van_bytes) < 12:
            continue

        if asset_hash not in fro_map:
            continue

        # Load Frontiers template
        entry = fro_map[asset_hash]
        fro_bytes = fro_esf_bytes[entry.offset : entry.offset + entry.length]

        # A. Parse node trees
        try:
            van_node, _ = parse_node(van_bytes, 0)
            fro_node, _ = parse_node(fro_bytes, 0)
        except Exception as e:
            print(f"  [-] Parse error on asset {asset_label}: {e}")
            continue

        # B. Perform texture swap on the Frontiers template node tree
        van_tex_containers = [c for c in van_node['children'] if c['type_id'] in (0x11100, 0x11110)]
        fro_tex_containers = [c for c in fro_node.get('children', []) if c['type_id'] == 0x11110]

        graft_root = copy.deepcopy(fro_node)
        if van_tex_containers and fro_tex_containers:
            fro_tex_idx = next(i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x11110)
            graft_root['children'][fro_tex_idx] = translate_texture_container(
                vanilla_container   = van_tex_containers[0],
                frontiers_container = fro_tex_containers[0],
                asset_label         = asset_label
            )
            update_node_sizes(graft_root)

        # Serialize texture-grafted Frontiers template
        donor_bytes = serialize_node(graft_root)

        # C. Perform geometry splicing (Frontiers header + Vanilla VIF geometry)
        fro_mesh_start = find_mesh_start(donor_bytes, "donor")
        van_mesh_start = find_mesh_start(van_bytes, "target")

        if fro_mesh_start > -1 and van_mesh_start > -1:
            # Spliced hybrid payload
            header_bytes = bytearray(donor_bytes[0 : fro_mesh_start])
            geometry_bytes = van_bytes[van_mesh_start :]
            new_total = len(header_bytes) + len(geometry_bytes)

            # Patch size fields in the spliced header
            patch_size_fields(header_bytes, new_total, len(donor_bytes))

            final_payload = bytes(header_bytes) + geometry_bytes
            spliced += 1
        else:
            # Pure header/stub or palette-only node, use the texture-grafted donor tree directly
            final_payload = donor_bytes
            pure_headers += 1

        with open(filepath, 'wb') as f:
            f.write(final_payload)

        processed += 1

    print(f"\n[+] Master Graft Complete!")
    print(f"    Total Processed : {processed}")
    print(f"    Spliced meshes  : {spliced}")
    print(f"    Pure header stubs: {pure_headers}")

    # 4. Trigger ESF database rebuilder
    print("\n[*] Step 4: Rebuilding the merged CHAR.ESF database...")
    subprocess.run([sys.executable, "esf_rebuilder.py"], check=True)

    # 5. Trigger ISO repacker
    print("\n[*] Step 5: Repacking and patching the playable game ISO...")
    subprocess.run([sys.executable, "repack_iso.py"], check=True)

    # 6. Verify final ISO
    print("\n[*] Step 6: Verifying repacked ISO integrity...")
    subprocess.run([sys.executable, "verify_final_iso.py"], check=True)

    print("\n" + "=" * 70)
    print("  ALL DONE! MASTER PIPELINE SUCCESSFUL!")
    print("  The original classic models (geometry + textures) have been successfully ported.")
    print("=" * 70)

if __name__ == '__main__':
    main()
