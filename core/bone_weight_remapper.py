#!/usr/bin/env python3
"""
bone_weight_remapper.py
=======================
Parses PS2 payloads to remap the joint/bone indices embedded in the W-component
of V4-32 Vertex/Weight arrays. 
This prevents VU1 shader geometry collapse when transplanting a 32-bone mesh 
into a 43-bone skeleton framework.
"""

import os
import sys
import struct
import math

def parse_vif_code(code_32bit):
    imm = code_32bit & 0xFFFF
    num = (code_32bit >> 16) & 0xFF
    cmd = (code_32bit >> 24) & 0xFF
    return {'imm': imm, 'num': num, 'cmd': cmd}

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

def extract_bone_positions(root_node, expected_count):
    """
    Navigates the ESF tree to isolate the specific Skeleton definition node
    (e.g., 0x02710 or 0x0B070) and extracts the hierarchical joint array.
    Strictly validates that exactly `expected_count` matrices are found.
    """
    positions = []
    
    def search_skeleton(node):
        # Target the specific Skeleton definition node
        if node['type_id'] in (0x02710, 0x0B070, 0x02800):
            # Once inside the skeleton container, find the contiguous matrix array
            for child in node['children']:
                if child['child_count'] == 0 and child['inline_data']:
                    # PS2 Joint Matrices are typically 64 bytes (4x4 floats)
                    # or 80 bytes (4x4 floats + 16 byte name string)
                    data = child['inline_data']
                    sz = len(data)
                    
                    if sz == expected_count * 64:
                        for i in range(expected_count):
                            x, y, z = struct.unpack_from('<fff', data, (i * 64) + 48)
                            positions.append((x, y, z))
                        return True
                    elif sz == expected_count * 80:
                        for i in range(expected_count):
                            x, y, z = struct.unpack_from('<fff', data, (i * 80) + 48)
                            positions.append((x, y, z))
                        return True
                        
        for child in node['children']:
            if search_skeleton(child):
                return True
        return False
        
    search_skeleton(root_node)
    
    if len(positions) != expected_count:
        print(f"[-] CRITICAL ERROR: Skeleton extraction failed. Expected {expected_count} matrices, but extracted {len(positions)} matrices.")
        print("[*] FALLBACK: Executing hex-dump of skeleton candidate nodes for manual stride inspection...")
        
        def dump_skeleton(node):
            if node['type_id'] in (0x02710, 0x0B070, 0x02800):
                print(f"  -> Found Skeleton Candidate 0x{node['type_id']:05X} (Size: {node['data_size']})")
                def dump_leaves(child_node, depth=1):
                    if child_node['child_count'] == 0 and child_node['inline_data']:
                        data = child_node['inline_data']
                        sz = len(data)
                        print(f"     {'  '*depth}[Leaf 0x{child_node['type_id']:05X}] Size: {sz} bytes")
                        dump_sz = min(sz, 64)
                        hex_str = " ".join([f"{b:02X}" for b in data[:dump_sz]])
                        print(f"     {'  '*depth}  Hex: {hex_str}")
                    for c in child_node['children']:
                        dump_leaves(c, depth+1)
                for child in node['children']:
                    dump_leaves(child)
            for child in node['children']:
                dump_skeleton(child)
                
        dump_skeleton(root_node)
        raise ValueError(f"CRITICAL ERROR: Skeleton extraction failed. Expected {expected_count} matrices, but extracted {len(positions)} matrices. Aborting to prevent geometry collapse.")
        
    return positions

def build_translation_dictionary(van_positions, fro_positions):
    """
    Builds an index map {vanilla_index: frontiers_index} by finding the nearest 
    spatial neighbor in 3D space.
    """
    mapping = {}
    print(f"[*] Building translation dictionary: {len(van_positions)} Vanilla Bones -> {len(fro_positions)} Frontiers Bones")
    
    # If we couldn't extract positions due to format changes, fallback to 1:1 map
    if len(van_positions) == 0 or len(fro_positions) == 0:
        print("    [!] FATAL ERROR: Spatial data not found. Cannot calculate valid distance matrix.")
        print("    [!] Aborting to prevent geometry collapse.")
        sys.exit(1)

    for v_idx, v_pos in enumerate(van_positions):
        min_dist = float('inf')
        best_f_idx = v_idx
        
        for f_idx, f_pos in enumerate(fro_positions):
            dist = math.dist(v_pos, f_pos)
            if dist < min_dist:
                min_dist = dist
                best_f_idx = f_idx
                
        mapping[v_idx] = best_f_idx
        print(f"    [+] Mapped Vanilla Joint {v_idx} -> Frontiers Joint {best_f_idx} (Dist: {min_dist:.4f})")
        
    return mapping

def remap_v4_32_weights(data_bytes, index_map):
    """
    Scans the binary for 0x6C (V4-32) unpacking commands.
    Reads the W component, extracts the bone index, shifts it using index_map, 
    and repacks it.
    """
    data = bytearray(data_bytes)
    pos = 0
    file_len = len(data)
    patched_count = 0
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', data, pos)[0]
        vif = parse_vif_code(code)
        
        if vif['cmd'] == 0x6C and vif['num'] > 0:
            align_offset = (pos + 15) & ~15
            num = vif['num']
            
            for i in range(num):
                v_offset = align_offset + (i * 16)
                if v_offset + 16 <= file_len:
                    # In PS2, W component often holds the index. It can be a float or int.
                    # We read it as an unsigned integer directly from the 4th 32-bit slot.
                    w_int = struct.unpack_from('<I', data, v_offset + 12)[0]
                    
                    # Heuristic: Is it a direct bone index? (0 to ~128)
                    # Or is it multiplied by 4? (VU1 matrix addressing)
                    is_mul4 = False
                    bone_idx = w_int
                    
                    if 0 <= w_int < 256 and w_int % 4 == 0 and w_int > 0:
                        bone_idx = w_int // 4
                        is_mul4 = True
                        
                    if 0 <= bone_idx < 128:  # Plausible bone index
                        if bone_idx in index_map:
                            new_idx = index_map[bone_idx]
                            
                            # Re-multiply by 4 if necessary
                            new_w_int = new_idx * 4 if is_mul4 else new_idx
                            
                            if new_w_int != w_int:
                                struct.pack_into('<I', data, v_offset + 12, new_w_int)
                                patched_count += 1
                                
            pos = align_offset + (num * 16) - 4
        pos += 4
        
    return bytes(data), patched_count

def process_file(payload_path, vanilla_esf_path, frontiers_esf_path):
    print("=" * 80)
    print("  PS2 VU1 BONE WEIGHT REMAPPER")
    print("=" * 80)
    print(f"[*] Target Payload: {os.path.basename(payload_path)}")
    
    with open(payload_path, 'rb') as f:
        payload_data = f.read()
        
    van_positions = []
    fro_positions = []
    
    # Attempt to parse skeletons for spatial mapping
    if os.path.exists(vanilla_esf_path):
        with open(vanilla_esf_path, 'rb') as f:
            van_root, _ = parse_node(f.read(), 0)
            van_positions = extract_bone_positions(van_root, 32)
            
    if os.path.exists(frontiers_esf_path):
        with open(frontiers_esf_path, 'rb') as f:
            fro_root, _ = parse_node(f.read(), 0)
            fro_positions = extract_bone_positions(fro_root, 43)
            
    # Generate Map
    index_map = build_translation_dictionary(van_positions, fro_positions)
    
    def process_tree(node):
        patched = 0
        if node['child_count'] == 0 and node['inline_data']:
            # Only scan if the node is large enough to contain VIF structures
            if len(node['inline_data']) > 16:
                new_data, count = remap_v4_32_weights(node['inline_data'], index_map)
                node['inline_data'] = new_data
                patched += count
        for child in node['children']:
            patched += process_tree(child)
        return patched
        
    payload_root, _ = parse_node(payload_data, 0)
    count = process_tree(payload_root)
    
    # Needs a simple serialize function
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
        
    patched_data = serialize_node(payload_root)
    print(f"[*] Successfully remapped {count} bone weight indices in the V4-32 arrays.")
    
    # Overwrite payload
    with open(payload_path, 'wb') as f:
        f.write(patched_data)
        
    print(f"[+] Payload saved and ready for DMA injection.")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python bone_weight_remapper.py <payload.bin> [vanilla.esf] [frontiers.esf]")
        sys.exit(1)
        
    payload = sys.argv[1]
    van_esf = sys.argv[2] if len(sys.argv) > 2 else "workspace/original/CHAR.ESF"
    fro_esf = sys.argv[3] if len(sys.argv) > 3 else "workspace/expansion/CHAR.ESF"
    
    if not os.path.exists(payload):
        print(f"[-] Error: Payload {payload} not found.")
        sys.exit(1)
        
    process_file(payload, van_esf, fro_esf)
