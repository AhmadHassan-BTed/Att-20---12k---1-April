import os
import sys
import json
import struct
import copy
import shutil
import subprocess

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

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

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 80)
    print("  PS2 CLEAN TRANSPLANT: MACRO-NODE INJECTION")
    print("=" * 80)
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    print("[*] Parsing Vanilla ESF...")
    with open(original_esf, 'rb') as f:
        van_bytes = f.read()
    van_map = {e.asset_id: e for e in ESFParser(van_bytes).parse().pointer_table if e.asset_id}
    
    print("[*] Parsing Frontiers ESF...")
    with open(expansion_esf, 'rb') as f:
        fro_bytes = f.read()
    fro_map = {e.asset_id: e for e in ESFParser(fro_bytes).parse().pointer_table if e.asset_id}
    
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    for t in targets:
        h = int(t['expansion_hash'], 16)
        print(f"\n  [+] Processing Target: 0x{h:08X}")
        
        van_entry = van_map[h]
        fro_entry = fro_map[h]
        
        van_node, _ = parse_node(van_bytes[van_entry.offset : van_entry.offset + van_entry.length], 0)
        fro_node, _ = parse_node(fro_bytes[fro_entry.offset : fro_entry.offset + fro_entry.length], 0)
        
        graft_root = copy.deepcopy(fro_node)
        
        # 1. Swap 0x02610 (Geometry) in its entirety
        van_geom = next((c for c in van_node['children'] if c['type_id'] == 0x02610), None)
        fro_geom_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x02610), None)
        
        if van_geom and fro_geom_idx is not None:
            graft_root['children'][fro_geom_idx] = copy.deepcopy(van_geom)
            print(f"      -> Swapped entire 0x02610 Geometry Node (Preserving DMA Chains)")
        else:
            print("      -> [!] Error: Geometry Node missing!")
            
        # 2. Swap 0x02800 (Bone Matrices/Hierarchy) if it exists, to match the geometry
        van_bone = next((c for c in van_node['children'] if c['type_id'] == 0x02800), None)
        fro_bone_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x02800), None)
        if van_bone and fro_bone_idx is not None:
            graft_root['children'][fro_bone_idx] = copy.deepcopy(van_bone)
            print(f"      -> Swapped entire 0x02800 Bone Matrix Node (Matches Geometry)")
            
        van_0b070 = next((c for c in van_node['children'] if c['type_id'] == 0x0B070), None)
        fro_0b070_idx = next((i for i, c in enumerate(graft_root['children']) if c['type_id'] == 0x0B070), None)
        if van_0b070 and fro_0b070_idx is not None:
             graft_root['children'][fro_0b070_idx] = copy.deepcopy(van_0b070)
             print(f"      -> Swapped entire 0x0B070 Skeleton Node (Matches Geometry)")
             
        # But we KEEP 0x11110 (Materials) intact from Frontiers!
        print(f"      -> Retained Frontiers 0x11110 Material Node (Prevents Render Rejection)")
        
        # 3. Recalculate sizes
        update_node_sizes(graft_root)
        final_payload = serialize_node(graft_root)
        
        bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
        with open(bin_path, 'wb') as out_f:
            out_f.write(final_payload)
            
    print("\n[*] Rebuilding ESF...")
    subprocess.run([sys.executable, "-m", "core.esf_rebuilder"], check=True)
    
    shutil.copyfile("workspace/FINAL_CHAR_MERGED.ESF", "workspace/ISO_EXTRACTED/CHAR.ESF")
    
    print("\n[*] Compiling ISO...")
    subprocess.run([sys.executable, "core/bare_metal_build.py"], check=True)
    
    print("\n[+] Success! MANUAL_PATCH.iso is ready with Pristine Macro-Node Injection.")

if __name__ == "__main__":
    main()
