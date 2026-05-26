#!/usr/bin/env python3
"""
EQOA Surgical Format Grafter (Frankenstein Patcher)
===================================================
A production-grade, layout-aware surgical binary splicing patcher.
Transforms vanilla EQOA character model payloads into natively compatible 
EverQuest Online Adventures: Frontiers character model assets by updating 
the node tree structure to align exactly with the Frontiers engine's expectations.

Surgical Transformations:
1. Root type_id upgrade: 0x62700 -> 0x72700.
2. Bone/Skeleton transform node type_id upgrade: 0x12400 -> 0x22400.
3. Root child list expansion: appends Frontiers-specific nodes 0x02950 (size 0) 
   and 0x02960 (size 4, 0x00000000) to match the 17-child format layout.
4. Recursive data size recalculation across the entire tree.
"""

import os
import sys
import struct
import glob


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


def patch_payload(filepath):
    """Surgically parse, update structural nodes, resize, and serialize a payload file."""
    filename = os.path.basename(filepath)
    
    with open(filepath, 'rb') as f:
        data = f.read()
        
    # Check if this is a master model payload
    if len(data) < 12:
        return False
        
    type_id = struct.unpack_from('<I', data, 0)[0]
    if type_id not in (0x62700, 0x72700):
        # Skip texture or dependency sub-assets which do not start with character model root types
        return False
        
    try:
        root, _ = parse_node(data, 0)
    except Exception as e:
        print(f"  [-] Failed to parse node tree of {filename}: {e}")
        return False
        
    was_patched = False
    
    # 1. Root type_id upgrade
    if root['type_id'] == 0x62700:
        root['type_id'] = 0x72700
        was_patched = True
        
    # 2. Upgrade Bone/Skeleton transform node type_id: 0x12400 -> 0x22400
    for child in root['children']:
        if child['type_id'] == 0x12400:
            child['type_id'] = 0x22400
            was_patched = True
            
    # 3. Add Frontiers-specific trailer child nodes (0x02950 and 0x02960) to expand 15 -> 17 children
    if len(root['children']) == 15:
        # Check current child types to be absolutely safe
        child_types = [c['type_id'] for c in root['children']]
        if child_types[-1] == 0x02940:
            # Create Child 15: type 0x02950, size 0
            child15 = {
                'type_id': 0x02950,
                'data_size': 0,
                'child_count': 0,
                'children': [],
                'inline_data': b''
            }
            # Create Child 16: type 0x02960, size 4
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
            was_patched = True
            
    if was_patched:
        # 4. Recalculate sizes recursively
        update_node_sizes(root)
        
        # 5. Serialize and overwrite payload file
        patched_data = serialize_node(root)
        with open(filepath, 'wb') as f:
            f.write(patched_data)
            
        print(f"  [+] Surgically Grafted {filename}:")
        print(f"      - Root Node: 0x62700 -> 0x72700")
        print(f"      - Children Count: 15 -> 17 (Added 0x02950 and 0x02960)")
        print(f"      - Skeleton Transforms Node: 0x12400 -> 0x22400")
        print(f"      - Original Size: {len(data):,} bytes | New Size: {len(patched_data):,} bytes")
        return True
    else:
        return False


def main():
    payload_dir = './workspace/payloads'
    print("[*] Commencing Surgical Frontiers Engine Formatting...")
    
    bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
    if not bin_files:
        print("[-] Error: No payloads found in workspace/payloads directory.")
        sys.exit(1)
        
    grafted_count = 0
    for filepath in bin_files:
        success = patch_payload(filepath)
        if success:
            grafted_count += 1
            
    print(f"\n[+] FORMATTING COMPLETE: {grafted_count} master payloads successfully updated for the Frontiers engine!")


if __name__ == '__main__':
    main()
