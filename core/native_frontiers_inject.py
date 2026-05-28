#!/usr/bin/env python3
"""
native_frontiers_inject.py
==========================
Correct injection pipeline that uses the NATIVE Frontiers 0x72700 models
as-is (no geometry transplant) to avoid skeleton/bone matrix mismatches.

Previous pipelines attempted to swap Vanilla 0x62700 geometry (0x02610) into 
Frontiers 0x72700 wrappers, but this caused invisible rendering because:
  1. 62 of 72 mesh parts have DIFFERENT sizes between Vanilla and Frontiers
  2. The bone matrices (0x02800) differ: Van=76,274 vs Fro=95,614 bytes
  3. The vertex bone weight indices are bound to the Vanilla skeleton layout
     but the skeleton used is from Frontiers → VU1 reads invalid bone matrices
     → vertices at garbage positions → invisible model

The correct approach: The Frontiers CHAR.ESF already contains the native
0x72700 versions of these 11 models. We simply use them unmodified.
"""
import os
import sys
import json
import struct
import copy
import shutil
import subprocess

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser


def parse_node(data, pos):
    if pos + 12 > len(data): return None, pos
    tid = struct.unpack_from('<I', data, pos)[0]
    dsz = struct.unpack_from('<I', data, pos + 4)[0]
    cc  = struct.unpack_from('<I', data, pos + 8)[0]
    node = {'type_id': tid, 'data_size': dsz, 'child_count': cc, 'children': [], 'inline_data': None}
    pos += 12
    if cc == 0:
        node['inline_data'] = data[pos:pos + dsz]
        pos += dsz
    else:
        for _ in range(cc):
            child, pos = parse_node(data, pos)
            if child: node['children'].append(child)
    return node, pos


def serialize_node(node):
    buf = bytearray()
    buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    if node['child_count'] == 0:
        if node['inline_data']:
            buf += node['inline_data']
    else:
        for child in node['children']:
            buf += serialize_node(child)
    return bytes(buf)


def update_node_sizes(node):
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data']) if node['inline_data'] else 0
    else:
        node['child_count'] = len(node['children'])
        total = 0
        for child in node['children']:
            update_node_sizes(child)
            total += 12 + child['data_size']
        node['data_size'] = total


def main():
    json_path = 'workspace/target_assets.json'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 80)
    print("  NATIVE FRONTIERS INJECTION PIPELINE")
    print("  (Uses Frontiers 0x72700 models AS-IS — no Vanilla geometry transplant)")
    print("=" * 80)
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
    
    # Parse Frontiers ESF
    print("\n[*] Parsing Frontiers CHAR.ESF...")
    with open(expansion_esf, 'rb') as f:
        fro_bytes = f.read()
    fro_parser = ESFParser(fro_bytes).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id}
    
    # Clean and recreate payloads directory
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    print(f"\n[*] Extracting {len(targets)} native Frontiers models as payloads...")
    
    for idx, t in enumerate(targets):
        h = int(t['expansion_hash'], 16)
        fro_entry = fro_map[h]
        
        # Extract the native Frontiers model bytes directly
        fro_data = fro_bytes[fro_entry.offset:fro_entry.offset + fro_entry.length]
        
        # Verify it parses correctly
        root, end = parse_node(fro_data, 0)
        if root is None:
            print(f"  [{idx+1}/11] FAILED: Could not parse 0x{h:08X}")
            sys.exit(1)
        
        # Validate: it should be 0x72700 with 17 children
        assert root['type_id'] == 0x72700, f"Expected 0x72700 but got 0x{root['type_id']:05X}"
        
        # Save as payload (unmodified native Frontiers model)
        bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
        with open(bin_path, 'wb') as f:
            f.write(fro_data)
        
        # Report
        geom = next((c for c in root['children'] if c['type_id'] == 0x2610), None)
        skel = next((c for c in root['children'] if c['type_id'] == 0x0B070), None)
        mat  = next((c for c in root['children'] if c['type_id'] == 0x02800), None)
        
        geom_parts = geom['child_count'] if geom else 0
        skel_children = skel['child_count'] if skel else 0
        mat_size = mat['data_size'] if mat else 0
        
        print(f"  [{idx+1}/11] 0x{h:08X} | "
              f"Root=0x{root['type_id']:05X} | "
              f"Size={len(fro_data):,} | "
              f"Geom={geom_parts} parts | "
              f"Skel={skel_children} nodes | "
              f"BoneMat={mat_size:,} B")
    
    # Now rebuild the ESF
    print("\n[*] Rebuilding ESF database...")
    subprocess.run([sys.executable, "-m", "core.esf_rebuilder"], check=True)
    
    # Copy to ISO_EXTRACTED
    shutil.copyfile("workspace/FINAL_CHAR_MERGED.ESF", "workspace/ISO_EXTRACTED/CHAR.ESF")
    
    # Build ISO
    print("\n[*] Building final ISO...")
    subprocess.run([sys.executable, "core/bare_metal_build.py"], check=True)
    
    print("\n" + "=" * 80)
    print("  NATIVE FRONTIERS INJECTION COMPLETE!")
    print("  All 11 models use their native Frontiers geometry, skeleton, and bone matrices.")
    print("  No Vanilla->Frontiers geometry transplant was performed.")
    print("  MANUAL_PATCH.iso is ready for PCSX2 testing.")
    print("=" * 80)


if __name__ == '__main__':
    main()
