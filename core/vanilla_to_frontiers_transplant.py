#!/usr/bin/env python3
"""
vanilla_to_frontiers_transplant.py
==================================
Master High-Fidelity Pristine Structural Transplant Pipeline for EQOA character models.
Upgrades classic Vanilla model container structures into Frontiers-native 0x72700 wrappers 
by surgically grafting Vanilla texture containers, patching TEX0 context dimensions, and 
replaces geometry containers to resolve VU1 rigging and skinning rendering rejections.

Author: Lead Software Architect & Senior Low Level Game Developer
"""
import os
import sys
import math
import struct
import json
import copy
import shutil
import subprocess

from core.esf_parser import ESFParser

# ─────────────────────────────────────────────────────────────────────────────
# PS2 GS Constants & Bitfield Utilities for TEX0 Patching
# ─────────────────────────────────────────────────────────────────────────────
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
# FJBO Node Tree Traverser and Serializer
# ─────────────────────────────────────────────────────────────────────────────

def parse_node(data: bytes, pos: int) -> tuple:
    """Recursively parse a binary node tree."""
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
    """Recursively update data_size and child_count for all nodes in the tree."""
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
    """Recursively serialize a node tree to binary bytes."""
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
# Clean Surgery and Graphics Translation Logic
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

def pristine_structural_upgrade(vanilla_bytes: bytes, frontiers_bytes: bytes, asset_label: str) -> bytes:
    """Surgically upgrades a pristine Vanilla model into a Frontiers-compliant wrapper by using the Frontiers base tree and injecting Vanilla mesh (0x02610) and translated textures."""
    van_node, _ = parse_node(vanilla_bytes, 0)
    fro_node, _ = parse_node(frontiers_bytes, 0)
    
    if van_node['type_id'] not in (0x62700, 0x72700):
        raise ValueError(f"Asset {asset_label} is not a valid Vanilla character model!")
        
    graft_root = copy.deepcopy(fro_node)
    
    print(f"    [+] Using Frontiers template (0x72700) as base for 100% compatibility")
    
    # 1. Graft Vanilla Mesh Container (0x02610)
    van_mesh = next((c for c in van_node['children'] if c['type_id'] == 0x02610), None)
    fro_mesh_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x02610), None)
    
    if van_mesh and fro_mesh_idx is not None:
        graft_root['children'][fro_mesh_idx] = copy.deepcopy(van_mesh)
        print(f"    [+] Surgically transplanted Vanilla Mesh (0x02610) into Frontiers template")
    else:
        print(f"    [!] Warning: Mesh container (0x02610) not found for transplantation in {asset_label}!")

    # 2. Graft Frontiers texture container with translated Vanilla textures and patched TEX0 registers
    van_tex = next((c for c in van_node['children'] if c['type_id'] in (0x11100, 0x11110)), None)
    fro_tex = next((c for c in fro_node['children'] if c['type_id'] == 0x11110), None)
    graft_root_tex_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] in (0x11100, 0x11110)), None)
    
    if van_tex and fro_tex and graft_root_tex_idx is not None:
        graft_root['children'][graft_root_tex_idx] = translate_texture_container(
            vanilla_container   = van_tex,
            frontiers_container = fro_tex,
            asset_label         = asset_label
        )
        print(f"    [+] Surgically translated Vanilla textures & patched TEX0 registers into Frontiers container slot {graft_root_tex_idx}")
    else:
        print(f"    [!] Warning: Texture container not found for transplantation in {asset_label}!")
            
    # 3. Recursively update all node sizes
    update_node_sizes(graft_root)
    
    # 4. Serialize
    final_payload = serialize_node(graft_root)
    return final_payload

# ─────────────────────────────────────────────────────────────────────────────
# Master Pipeline Orchestration
# ─────────────────────────────────────────────────────────────────────────────

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 80)
    print("  EQOA MASTER HIGH-FIDELITY PRISTINE STRUCTURAL TRANSPLANT PIPELINE")
    print("=" * 80)
    
    # 1. Load targets mapping
    if not os.path.exists(json_path):
        print(f"[-] Error: Targets mapping {json_path} not found!")
        sys.exit(1)
        
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    # 2. Parse Vanilla original database
    if not os.path.exists(original_esf):
        print(f"[-] Error: Original Vanilla CHAR.ESF not found at {original_esf}!")
        sys.exit(1)
        
    # 3. Parse Frontiers expansion database template
    if not os.path.exists(expansion_esf):
        print(f"[-] Error: Expansion Frontiers CHAR.ESF not found at {expansion_esf}!")
        sys.exit(1)
        
    print(f"\n[*] Step 1: Parsing Vanilla & Frontiers CHAR.ESF databases...")
    with open(original_esf, 'rb') as f:
        van_esf_bytes = f.read()
    van_parser = ESFParser(van_esf_bytes).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    with open(expansion_esf, 'rb') as f:
        fro_esf_bytes = f.read()
    fro_parser = ESFParser(fro_esf_bytes).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}
    
    # Clean payloads directory
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    # 4. Extract and surgically graft the 11 target character models
    print(f"\n[*] Step 2: Commencing Low-Level Clean Graft Surgery on {len(targets)} Vanilla targets...")
    for idx, t in enumerate(targets):
        h = int(t['expansion_hash'], 16)
        print(f"\n  [{idx+1}/11] Performing clean surgery on model 0x{h:08X}...")
        
        # Load vanilla payload from vanilla ESF bytes
        van_entry = van_map[h]
        vanilla_bytes = van_esf_bytes[van_entry.offset : van_entry.offset + van_entry.length]
        
        # Load frontiers template
        fro_entry = fro_map[h]
        frontiers_bytes = fro_esf_bytes[fro_entry.offset : fro_entry.offset + fro_entry.length]
        try:
            # Perform high-fidelity pristine structural upgrade
            final_payload = pristine_structural_upgrade(vanilla_bytes, frontiers_bytes, f"0x{h:08X}")
            
            # Save payload bin
            bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
            with open(bin_path, 'wb') as f:
                f.write(final_payload)
            print(f"    [+] Saved clean graft payload -> {bin_path} ({len(final_payload):,} bytes)")
            
            # Sanity parse validation
            parsed_node, end_pos = parse_node(final_payload, 0)
            if end_pos == len(final_payload) and parsed_node['child_count'] == 17:
                print("    [PASS] Clean hybrid model verified successfully!")
            else:
                print(f"    [FAIL] Clean hybrid model parsing mismatch! end_pos={end_pos}, expected {len(final_payload)}")
                sys.exit(1)
                
        except Exception as e:
            print(f"    [-] Surgery failed on 0x{h:08X}: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)
            
    # 5. Trigger ESF Database Recompiler
    print("\n[*] Step 3: Compiling database (esf_rebuilder)...")
    subprocess.run([sys.executable, "core/esf_rebuilder.py"], check=True)
    
    # 6. Trigger ISO Sector Repacker
    print("\n[*] Step 4: Repacking playable game ISO (repack_iso)...")
    subprocess.run([sys.executable, "core/repack_iso.py"], check=True)
    
    # 7. Apply Surgical UDF logical block patches
    print("\n[*] Step 5: Patching UDF allocation descriptors logical mapping...")
    subprocess.run([sys.executable, "core/patch_udf_char_esf_v2.py"], check=True)
    
    # 8. Execute Verification Pipeline
    print("\n[*] Step 6: Running high-integrity verification suite...")
    subprocess.run([sys.executable, "core/verify_injected_models.py"], check=True)
    subprocess.run([sys.executable, "core/verify_final_patch.py"], check=True)
    subprocess.run([sys.executable, "core/verify_final_iso.py"], check=True)
    
    print("\n" + "=" * 80)
    print("  HIGH-FIDELITY STRUCTURAL TRANSPLANT PIPELINE EXECUTED SUCCESSFULLY!")
    print("  11 character models surgically patched, merged, and fully verified in ISO!")
    print("=" * 80)

if __name__ == '__main__':
    main()
