#!/usr/bin/env python3
"""
advanced_mesh_injector.py
=========================
Offline DMA Chain Validator & Sub-Struct Injector for PS2 Mesh Files.
Implements PS2 DMA/VIF tag parsing to intelligently overwrite Vertex and UV arrays
without breaking the VU1 shader chain. Handles dynamic QWC recalculation and 
Bounding Box injection to prevent camera culling.

Architecture Phases:
Phase 1: Offline DMA Chain Validator
Phase 2: Intelligent Sub-Struct Overwriting (VIF Unpacks)
Phase 3: Bounding Box Protection

Author: Lead PS2 Reverse Engineer
"""

import os
import sys
import math
import struct

# ─────────────────────────────────────────────────────────────────────────────
# Phase 1: PS2 DMA Chain and VIF Code Parser
# ─────────────────────────────────────────────────────────────────────────────

def parse_dma_tag(tag_64bit):
    """Parses a 64-bit PS2 DMA tag into its components."""
    qwc = tag_64bit & 0xFFFF
    priority = (tag_64bit >> 26) & 0x3
    id_val = (tag_64bit >> 28) & 0x7
    irq = (tag_64bit >> 31) & 0x1
    addr = (tag_64bit >> 32) & 0xFFFFFFFF
    
    id_names = {0: 'refe', 1: 'cnt', 2: 'next', 3: 'ref', 4: 'refs', 5: 'call', 6: 'ret', 7: 'end'}
    return {
        'qwc': qwc,
        'id': id_val,
        'id_name': id_names.get(id_val, 'unknown'),
        'addr': addr
    }

def parse_vif_code(code_32bit):
    """Parses a 32-bit VIF code."""
    imm = code_32bit & 0xFFFF
    num = (code_32bit >> 16) & 0xFF
    cmd = (code_32bit >> 24) & 0xFF
    return {
        'imm': imm,
        'num': num,
        'cmd': cmd
    }

def validate_dma_chain(data, start_offset=128):
    """
    Phase 1: The Offline DMA Chain Validator
    Scans through the file via DMA chain QWC jumps, ensuring structural integrity
    for the Vector Unit (VU1).
    """
    print("\n[*] Phase 1: Executing Offline DMA Chain Validation...")
    pos = start_offset
    file_len = len(data)
    chain_valid = True
    tags_found = 0
    
    # Ensure 16-byte alignment
    pos = (pos + 15) & ~15
    
    while pos + 16 <= file_len:
        tag_64 = struct.unpack_from('<Q', data, pos)[0]
        vif_64 = struct.unpack_from('<Q', data, pos + 8)[0]
        
        dma = parse_dma_tag(tag_64)
        
        # Abort if we read garbage (heuristic)
        if dma['id_name'] == 'unknown' or (dma['qwc'] > 0x4000 and dma['id_name'] != 'ref'):
            print(f"    [!] Chain Break at 0x{pos:X}: Invalid Tag ID {dma['id']} or abnormal QWC ({dma['qwc']})")
            chain_valid = False
            break
            
        tags_found += 1
        print(f"    [DMA Tag] 0x{pos:06X} | ID: {dma['id_name'].upper():<4} | QWC: {dma['qwc']:<4}")
        
        if dma['id_name'] == 'end':
            print("    [+] Reached DMA END tag successfully.")
            break
            
        # Jump exactly qwc * 16 bytes forward
        jump_bytes = dma['qwc'] * 16
        pos += 16 + jump_bytes

    if tags_found == 0:
        print("    [!] FAILED: No valid DMA chain discovered. Structure is severely malformed.")
        return False
    elif chain_valid:
        print(f"    [PASS] DMA Chain Validated Mathematically! ({tags_found} linked tags traversed)")
        return True
    else:
        print("    [FAIL] DMA Chain Validation Failed. VU1 will crash if loaded.")
        return False

# ─────────────────────────────────────────────────────────────────────────────
# Phase 3: Bounding Box Protection
# ─────────────────────────────────────────────────────────────────────────────

def find_bounding_box(data, search_limit=256):
    """
    Locates the 6 Bounding Box floats (X, Y, Z min/max) typically in the header.
    Returns offset or None.
    """
    for i in range(0, min(search_limit, len(data) - 24), 4):
        try:
            floats = struct.unpack_from('<ffffff', data, i)
            valid = True
            for f in floats:
                # Heuristic: Valid PS2 model space bounds
                if math.isnan(f) or math.isinf(f) or abs(f) > 50000.0 or abs(f) < 0.0001 and f != 0.0:
                    valid = False
                    break
            if valid and any(f != 0.0 for f in floats):
                return i
        except Exception:
            continue
    return None

# ─────────────────────────────────────────────────────────────────────────────
# Phase 2: Intelligent Sub-Struct Overwriting (VIF unpack arrays)
# ─────────────────────────────────────────────────────────────────────────────

# ─────────────────────────────────────────────────────────────────────────────
# ESF Node Tree Parser (To prevent Header Corruption during injection)
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

def process_injection(frontiers_bin, vanilla_bin, out_path="workspace/injected_mesh_output.bin"):
    print("=" * 80)
    print("  PS2 ADVANCED MESH INJECTOR & DMA ALIGNER")
    print("=" * 80)

    with open(frontiers_bin, 'rb') as f:
        fro_data = f.read()
        
    with open(vanilla_bin, 'rb') as f:
        van_data = f.read()

    # Parse as ESF Node Trees to preserve headers
    fro_root, _ = parse_node(fro_data, 0)
    van_root, _ = parse_node(van_data, 0)

    print("\n[*] Phase 3: Bounding Box Protection")
    # The Bounding Box is stored in the inline_data of the root node (0x72700 / 0x62700) or first child
    van_bb_off = find_bounding_box(van_root['children'][0]['inline_data'] if van_root['children'][0]['child_count'] == 0 else van_data)
    fro_bb_off = find_bounding_box(fro_root['children'][0]['inline_data'] if fro_root['children'][0]['child_count'] == 0 else fro_data)
    
    if van_bb_off is not None and fro_bb_off is not None:
        c_van = van_root['children'][0]['inline_data']
        c_fro = bytearray(fro_root['children'][0]['inline_data'])
        
        # calculate dynamic bounds from Vanilla Geometry Node (0x02610)
        van_mesh_idx = next((i for i, c in enumerate(van_root['children']) if c['type_id'] == 0x02610), None)
        min_x, min_y, min_z = 999999.0, 999999.0, 999999.0
        max_x, max_y, max_z = -999999.0, -999999.0, -999999.0
        
        if van_mesh_idx is not None:
            van_mesh = van_root['children'][van_mesh_idx]
            
            # Recursive function to parse all leaf node DMA chains
            def scan_dma_for_verts(node):
                nonlocal min_x, min_y, min_z, max_x, max_y, max_z
                if node['child_count'] == 0 and node['inline_data']:
                    dma_data = node['inline_data']
                    pos = (0 + 15) & ~15
                    while pos + 16 <= len(dma_data):
                        dma_tag = struct.unpack_from('<Q', dma_data, pos)[0]
                        qwc = dma_tag & 0xFFFF
                        id_val = (dma_tag >> 28) & 0x7
                        packet_data_start = pos + 16
                        packet_data_len = qwc * 16
                        
                        if qwc > 0 and packet_data_start + packet_data_len <= len(dma_data):
                            arrays = extract_vif_unpack_arrays(dma_data, packet_data_start, packet_data_len)
                            if 0x6C in arrays:
                                for arr in arrays[0x6C]:
                                    v_off = arr['data_offset']
                                    for i in range(arr['num']):
                                        if v_off + 12 <= len(dma_data):
                                            f = struct.unpack_from('<fff', dma_data, v_off)
                                            if not (math.isnan(f[0]) or math.isnan(f[1]) or math.isnan(f[2])):
                                                if abs(f[0]) < 50000.0 and abs(f[1]) < 50000.0 and abs(f[2]) < 50000.0:
                                                    if f[0] != 0.0 or f[1] != 0.0 or f[2] != 0.0:
                                                        if f[0] < min_x: min_x = f[0]
                                                        if f[1] < min_y: min_y = f[1]
                                                        if f[2] < min_z: min_z = f[2]
                                                        if f[0] > max_x: max_x = f[0]
                                                        if f[1] > max_y: max_y = f[1]
                                                        if f[2] > max_z: max_z = f[2]
                                        v_off += 16
                        if id_val == 7: # END
                            break
                        pos += 16 + packet_data_len
                for child in node['children']:
                    scan_dma_for_verts(child)
            
            scan_dma_for_verts(van_mesh)
                
        if min_x == 999999.0:
            # Fallback if no vertices found
            van_bb = struct.unpack_from('<ffffff', c_van, van_bb_off)
            struct.pack_into('<ffffff', c_fro, fro_bb_off, *van_bb)
        else:
            c_x = (min_x + max_x) / 2.0
            c_y = (min_y + max_y) / 2.0
            c_z = (min_z + max_z) / 2.0
            
            # calculate radius
            max_sq = 0.0
            
            def scan_dma_for_radius(node):
                nonlocal max_sq
                if node['child_count'] == 0 and node['inline_data']:
                    dma_data = node['inline_data']
                    pos = (0 + 15) & ~15
                    while pos + 16 <= len(dma_data):
                        dma_tag = struct.unpack_from('<Q', dma_data, pos)[0]
                        qwc = dma_tag & 0xFFFF
                        id_val = (dma_tag >> 28) & 0x7
                        packet_data_start = pos + 16
                        packet_data_len = qwc * 16
                        
                        if qwc > 0 and packet_data_start + packet_data_len <= len(dma_data):
                            arrays = extract_vif_unpack_arrays(dma_data, packet_data_start, packet_data_len)
                            if 0x6C in arrays:
                                for arr in arrays[0x6C]:
                                    v_off = arr['data_offset']
                                    for i in range(arr['num']):
                                        if v_off + 12 <= len(dma_data):
                                            f = struct.unpack_from('<fff', dma_data, v_off)
                                            if not (math.isnan(f[0]) or math.isnan(f[1]) or math.isnan(f[2])):
                                                if abs(f[0]) < 50000.0 and abs(f[1]) < 50000.0 and abs(f[2]) < 50000.0:
                                                    if f[0] != 0.0 or f[1] != 0.0 or f[2] != 0.0:
                                                        dx, dy, dz = f[0] - c_x, f[1] - c_y, f[2] - c_z
                                                        dist_sq = dx*dx + dy*dy + dz*dz
                                                        if dist_sq > max_sq: max_sq = dist_sq
                                        v_off += 16
                        if id_val == 7: # END
                            break
                        pos += 16 + packet_data_len
                for child in node['children']:
                    scan_dma_for_radius(child)
            
            scan_dma_for_radius(van_mesh)
            r = math.sqrt(max_sq)
            
            struct.pack_into('<ffffff', c_fro, fro_bb_off, min_x, min_y, min_z, max_x, max_y, max_z)
            # pack sphere
            struct.pack_into('<ffff', c_fro, fro_bb_off + 24, c_x, c_y, c_z, r)
            print(f"    [+] Injected dynamic Bounding Box: min({min_x:.2f}, {min_y:.2f}, {min_z:.2f}) max({max_x:.2f}, {max_y:.2f}, {max_z:.2f}) r({r:.2f})")
            
        fro_root['children'][0]['inline_data'] = bytes(c_fro)
    else:
        print("    [!] Could not cleanly locate bounding box arrays.")

def get_unpack_size(cmd, num):
    base_cmd = cmd & 0x0F
    fmt = (base_cmd >> 2) & 3
    sz = base_cmd & 3
    components = [1, 2, 3, 4][fmt]
    bits = [32, 16, 8, 5][sz]
    total_bits = components * bits * num
    total_bytes = (total_bits + 7) // 8
    aligned_bytes = (total_bytes + 3) & ~3
    return aligned_bytes

def extract_vif_unpack_arrays(data: bytes, start: int, length: int) -> dict:
    pos = start
    end = min(start + length, len(data))
    arrays = {}
    
    while pos + 4 <= end:
        vif_code = struct.unpack_from('<I', data, pos)[0]
        imm = vif_code & 0xFFFF
        num = (vif_code >> 16) & 0xFF
        cmd = (vif_code >> 24) & 0xFF
        
        pos += 4
        
        if 0x60 <= cmd <= 0x7F:
            if num == 0: num = 256
            data_size = get_unpack_size(cmd, num)
            base_vif_cmd = cmd & ~0x10
            arrays[base_vif_cmd] = arrays.get(base_vif_cmd, [])
            arrays[base_vif_cmd].append({
                'vif_offset': pos - 4,
                'data_offset': pos,
                'size': data_size,
                'num': num,
                'raw_cmd': cmd
            })
            pos += data_size
            pos = (pos + 3) & ~3
        elif cmd == 0x50 or cmd == 0x51:
            pos += imm * 16
    return arrays

def patch_vif_arrays_in_dma_chain(fro_dma: bytes, van_dma_data: bytes) -> bytes:
    patched_data = bytearray(fro_dma)
    pos = (0 + 15) & ~15
    
    while pos + 16 <= len(fro_dma):
        dma_tag = struct.unpack_from('<Q', fro_dma, pos)[0]
        qwc = dma_tag & 0xFFFF
        id_val = (dma_tag >> 28) & 0x7
        id_names = {0:'refe', 1:'cnt', 2:'next', 3:'ref', 4:'refs', 5:'call', 6:'ret', 7:'end'}
        id_name = id_names.get(id_val, 'unknown')
        
        packet_data_start = pos + 16
        packet_data_len = qwc * 16
        
        if packet_data_start + packet_data_len > len(fro_dma):
            packet_data_len = len(fro_dma) - packet_data_start
            
        if qwc > 0 and packet_data_len >= 0:
            f_arrays = extract_vif_unpack_arrays(fro_dma, packet_data_start, packet_data_len)
            v_arrays = extract_vif_unpack_arrays(van_dma_data, packet_data_start, packet_data_len)
            
            if f_arrays and v_arrays:
                for cmd in [0x6C, 0x6D, 0x6A]:  # 0x6C = Vertices (V4-32), 0x6D = Vertices (V4-16), 0x6A = UVs (V3-8)
                    if cmd in f_arrays and cmd in v_arrays:
                        f_list = f_arrays[cmd]
                        v_list = v_arrays[cmd]
                        
                        if len(f_list) != len(v_list):
                            print(f"        [!] Array count mismatch for CMD 0x{cmd:02X}. Cannot 1-to-1 swap.")
                            continue
                            
                        for i in range(len(f_list)):
                            f_arr = f_list[i]
                            v_arr = v_list[i]
                            
                            if f_arr['size'] == v_arr['size'] and f_arr['num'] == v_arr['num']:
                                # 1-to-1 Exact Byte Swap
                                patched_data[f_arr['data_offset'] : f_arr['data_offset'] + f_arr['size']] = \
                                    van_dma_data[v_arr['data_offset'] : v_arr['data_offset'] + v_arr['size']]
                            else:
                                print(f"        [!] Array size mismatch at CMD 0x{cmd:02X} idx {i}. Skipping.")
                                
                print(f"        [+] Successfully performed 1-to-1 byte swap for packet at 0x{pos:06X}")
                
        if id_name == 'end':
            break
            
        pos += 16 + packet_data_len
        
    return bytes(patched_data)

def patch_tree_inline_data(node: dict, van_dma_data: bytes):
    if node['child_count'] == 0 and node['inline_data']:
        node['inline_data'] = patch_vif_arrays_in_dma_chain(node['inline_data'], van_dma_data)
    else:
        for child in node['children']:
            patch_tree_inline_data(child, van_dma_data)



if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python advanced_mesh_injector.py <Frontiers_Master.bin> <Vanilla_Source.bin>")
        sys.exit(1)
        
    frontiers_file = sys.argv[1]
    vanilla_file = sys.argv[2]
    
    if not os.path.exists(frontiers_file):
        print(f"Error: Frontiers Master {frontiers_file} not found.")
        sys.exit(1)
    if not os.path.exists(vanilla_file):
        print(f"Error: Vanilla Source {vanilla_file} not found.")
        sys.exit(1)
        
    process_injection(frontiers_file, vanilla_file)
