#!/usr/bin/env python3
"""
vanilla_to_frontiers_transplant.py
==================================
Master Pristine Structural Transplant Pipeline for EQOA character models.
Upgrades full classic Vanilla model container structures into Frontiers-native 
0x72700 wrappers to eliminate VU1 rigging and vertex-skinning mismatches.

Author: Lead Software Architect & Senior Low Level Game Developer
"""
import os
import sys
import struct
import json
import copy
import shutil
import subprocess

from core.esf_parser import ESFParser

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
# Master Pipeline Orchestration
# ─────────────────────────────────────────────────────────────────────────────

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 80)
    print("  EQOA MASTER PRISTINE STRUCTURAL TRANSPLANT PIPELINE")
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
        
    print(f"\n[*] Step 1: Parsing Vanilla CHAR.ESF...")
    with open(original_esf, 'rb') as f:
        van_esf_bytes = f.read()
    van_parser = ESFParser(van_esf_bytes).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    # Clean payloads directory
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    # 3. Extract and structurally upgrade the 11 target character models
    print(f"\n[*] Step 2: Commencing Structural Upgrades on {len(targets)} Vanilla targets...")
    for idx, t in enumerate(targets):
        h = int(t['original_hash'], 16)
        print(f"\n  [{idx+1}/11] Upgrading Vanilla model 0x{h:08X}...")
        
        # Extract pristine bytes
        van_entry = van_map[h]
        van_model_bytes = van_esf_bytes[van_entry.offset : van_entry.offset + van_entry.length]
        
        # Parse tree
        root, _ = parse_node(van_model_bytes, 0)
        
        # A. Root Upgrade: 0x62700 -> 0x72700
        if root['type_id'] == 0x62700:
            root['type_id'] = 0x72700
            print("    [+] Upgraded model root type to 0x72700 (Frontiers Character)")
            
        # B. Bone Node Upgrade: 0x12400 -> 0x22400
        bone_upgraded = False
        for child in root['children']:
            if child['type_id'] == 0x12400:
                child['type_id'] = 0x22400
                bone_upgraded = True
        if bone_upgraded:
            print("    [+] Upgraded skeleton bones type to 0x22400")
            
        # C. Expand Child List (15 -> 17) and append Frontiers trailer nodes
        if len(root['children']) == 15:
            # Child 15: type 0x02950, size 0
            child15 = {
                'type_id': 0x02950,
                'data_size': 0,
                'child_count': 0,
                'children': [],
                'inline_data': b''
            }
            # Child 16: type 0x02960, size 4, value 0x00000000
            child16 = {
                'type_id': 0x02960,
                'data_size': 4,
                'child_count': 0,
                'children': [],
                'inline_data': b'\x00\x00\x00\x00'
            }
            root['children'].append(child15)
            root['children'].append(child16)
            root['child_count'] = 17
            print("    [+] Appended trailer children 0x02950 and 0x02960 (expanded to 17 children)")
            
        # D. Recalculate sizes and serialize
        update_node_sizes(root)
        final_payload = serialize_node(root)
        
        # Save payload bin
        bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
        with open(bin_path, 'wb') as f:
            f.write(final_payload)
        print(f"    [+] Saved pristine structural graft -> {bin_path} ({len(final_payload):,} bytes)")
        
        # Sanity parse validation
        parsed_node, end_pos = parse_node(final_payload, 0)
        if end_pos == len(final_payload) and parsed_node['child_count'] == 17:
            print("    [PASS] Hybrid model verified successfully!")
        else:
            print("    [FAIL] Hybrid model parsing mismatch!")
            sys.exit(1)
            
    # 4. Trigger ESF Database Recompiler
    print("\n[*] Step 3: Compiling database (esf_rebuilder)...")
    subprocess.run([sys.executable, "core/esf_rebuilder.py"], check=True)
    
    # 5. Trigger ISO Sector Repacker
    print("\n[*] Step 4: Repacking playable game ISO (repack_iso)...")
    subprocess.run([sys.executable, "core/repack_iso.py"], check=True)
    
    # 6. Apply Surgical UDF logical block patches
    print("\n[*] Step 5: Patching UDF allocation descriptors logical mapping...")
    subprocess.run([sys.executable, "core/patch_udf_char_esf_v2.py"], check=True)
    
    # 7. Execute Verification Pipeline
    print("\n[*] Step 6: Running high-integrity verification suite...")
    subprocess.run([sys.executable, "core/verify_injected_models.py"], check=True)
    subprocess.run([sys.executable, "core/verify_final_patch.py"], check=True)
    subprocess.run([sys.executable, "core/verify_final_iso.py"], check=True)
    
    print("\n" + "=" * 80)
    print("  PRISTINE STRUCTURAL TRANSPLANT PIPELINE EXECUTED SUCCESSFULLY!")
    print("  11 character models structurally patched and fully verified in ISO!")
    print("=" * 80)

if __name__ == '__main__':
    main()
