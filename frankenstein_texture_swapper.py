#!/usr/bin/env python3
"""
EQOA Surgical Texture/Palette Swapper
=====================================
A production-grade, layout-aware surgical binary texture patcher.
Transplants Vanilla character textures and palettes into native Frontiers
skeletons to preserve original visual aesthetics while ensuring 100% native
compatibility with the Frontiers 43-bone skinning pipeline and Vector Unit engine.
"""

import os
import sys
import struct
import glob
from esf_parser import ESFParser

def parse_node(data, pos):
    """Recursively parse a binary node tree."""
    if pos + 12 > len(data):
        return None, pos
        
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node = {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'children': [],
        'inline_data': None
    }
    
    next_pos = pos + 12
    if child_count == 0:
        if next_pos + data_size > len(data):
            raise EOFError(f"Unexpected EOF reading leaf node at 0x{next_pos:X} (expected {data_size} bytes)")
        node['inline_data'] = data[next_pos : next_pos + data_size]
        next_pos += data_size
    else:
        for _ in range(child_count):
            child, next_pos = parse_node(data, next_pos)
            if child is not None:
                node['children'].append(child)
                
    return node, next_pos

def update_node_sizes(node):
    """Recursively calculate and update correct data_size for every node in the tree."""
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data'])
    else:
        size = 0
        for child in node['children']:
            update_node_sizes(child)
            size += 12 + child['data_size']
        node['data_size'] = size

def serialize_node(node):
    """Recursively serialize a node tree to binary bytes."""
    data = bytearray()
    header = struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    data.extend(header)
    
    if node['child_count'] == 0:
        if node['inline_data'] is not None:
            data.extend(node['inline_data'])
    else:
        for child in node['children']:
            data.extend(serialize_node(child))
            
    return bytes(data)

def perform_texture_swaps():
    payload_dir = './workspace/payloads'
    frontiers_esf_path = './workspace/expansion/CHAR.ESF'
    
    print("[*] Commencing Surgical Texture/Palette Splicing...")
    
    if not os.path.exists(frontiers_esf_path):
        print(f"[-] Error: Frontiers ESF not found at '{frontiers_esf_path}'")
        sys.exit(1)
        
    print("[*] Parsing Frontiers CHAR.ESF to locate native templates...")
    with open(frontiers_esf_path, 'rb') as f:
        frontiers_data = f.read()
    frontiers_parser = ESFParser(frontiers_data).parse()
    
    # Create index mapping of asset_hash -> PointerTableEntry in Frontiers ESF
    frontiers_map = { entry.asset_id: entry for entry in frontiers_parser.pointer_table if entry.asset_id is not None }
    
    bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
    if not bin_files:
        print("[-] Error: No payloads found in workspace/payloads directory.")
        sys.exit(1)
        
    swapped_count = 0
    skipped_count = 0
    
    for filepath in bin_files:
        filename = os.path.basename(filepath)
        try:
            hash_str = filename.split('_')[1].split('.')[0]
            asset_hash = int(hash_str, 16)
        except Exception:
            continue
            
        with open(filepath, 'rb') as f:
            vanilla_bytes = f.read()
            
        if len(vanilla_bytes) < 12:
            continue
            
        # Parse Vanilla Node
        try:
            vanilla_node, _ = parse_node(vanilla_bytes, 0)
        except Exception as e:
            print(f"  [-] Failed to parse Vanilla payload {filename}: {e}")
            continue
            
        # We transplant textures for any branch model node
        if vanilla_node['child_count'] == 0:
            continue
            
        # Check if texture container (type 0x11110) exists in Vanilla
        vanilla_tex_containers = [c for c in vanilla_node['children'] if c['type_id'] == 0x11110]
        if not vanilla_tex_containers:
            continue
        vanilla_tex = vanilla_tex_containers[0]
        
        # Check if asset hash exists in Frontiers
        if asset_hash not in frontiers_map:
            print(f"  [-] Hash 0x{asset_hash:08X} does not exist in Frontiers. Retaining pristine Vanilla format.")
            skipped_count += 1
            continue
            
        # Extract native Frontiers model
        entry = frontiers_map[asset_hash]
        frontiers_bytes = frontiers_data[entry.offset : entry.offset + entry.length]
        
        try:
            frontiers_node, _ = parse_node(frontiers_bytes, 0)
        except Exception as e:
            print(f"  [-] Failed to parse native Frontiers model for 0x{asset_hash:08X}: {e}")
            continue
            
        # Locate texture container node index in Frontiers
        fro_tex_indices = [i for i, c in enumerate(frontiers_node['children']) if c['type_id'] == 0x11110]
        if not fro_tex_indices:
            print(f"  [-] Warning: Native Frontiers model 0x{asset_hash:08X} lacks texture container node!")
            continue
        idx = fro_tex_indices[0]
        
        # Perform Surgical Transplant
        frontiers_node['children'][idx] = vanilla_tex
        
        # Recalculate sizes recursively across Frontiers tree
        update_node_sizes(frontiers_node)
        
        # Serialize the resulting hybrid node tree
        swapped_bytes = serialize_node(frontiers_node)
        
        # Overwrite the payload file with the hybrid model
        with open(filepath, 'wb') as f:
            f.write(swapped_bytes)
            
        print(f"  [+] Surgically Grafted textures onto native Frontiers skeleton for 0x{asset_hash:08X}:")
        print(f"      - Original Vanilla size:   {len(vanilla_bytes):,} bytes")
        print(f"      - Frontiers template size: {len(frontiers_bytes):,} bytes")
        print(f"      - Hybrid swapped size:     {len(swapped_bytes):,} bytes")
        swapped_count += 1
        
    print(f"\n[+] TEXTURE SPLICING COMPLETE: {swapped_count} models grafted with original Vanilla aesthetics!")
    print(f"    (Skipped {skipped_count} Vanilla-only sub-assets/dependencies to preserve backward compatibility)")

if __name__ == '__main__':
    perform_texture_swaps()
