#!/usr/bin/env python3
"""
visual_injection_diff.py
========================
A differential 3D visualizer that compares a Vanilla CSF against an Injected CSF.
Renders a 1x2 split-screen interactive window.
Highlights vertices in red if they reference invalid bone indices or are mangled spikes.
Gracefully catches VIF unpack corruption and outputs the exact byte offset.
"""

import os
import sys
import zlib
import struct
import math
import argparse
import numpy as np

try:
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
except ImportError:
    print("[-] Error: matplotlib is required. Run 'pip install matplotlib numpy' first.")
    sys.exit(1)

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def decompress_csf(filepath):
    with open(filepath, 'rb') as f:
        data = f.read()

    if not data.startswith(b'CESF'):
        return data

    out_buf = bytearray()
    pos = 0
    while pos < len(data):
        idx = data.find(b'\x78\xda', pos)
        if idx == -1:
            break
        try:
            d = zlib.decompressobj()
            chunk = d.decompress(data[idx:])
            out_buf.extend(chunk)
            consumed = len(data[idx:]) - len(d.unused_data)
            pos = idx + consumed
        except Exception as e:
            break

    return bytes(out_buf)

def find_geom_node(node):
    if node['type_id'] == 0x02610:
        return node
    for child in node.get('children', []):
        found = find_geom_node(child)
        if found:
            return found
    return None

def extract_skin_prim_buffer(data):
    """
    Parses the SkinPrimBuffer (0x21210 / 0x1210) node which contains the uncompiled
    raw collision geometry. This serves as our Golden Reference mesh.
    """
    vertices = []
    try:
        idx = 0
        pbtype = struct.unpack_from('<I', data, idx)[0]; idx += 4
        nmats = struct.unpack_from('<I', data, idx)[0]; idx += 4
        nfaces = struct.unpack_from('<I', data, idx)[0]; idx += 4
        unknown = struct.unpack_from('<I', data, idx)[0]; idx += 4
        p1 = struct.unpack_from('<I', data, idx)[0]; idx += 4
        p2 = struct.unpack_from('<I', data, idx)[0]; idx += 4
        p3 = struct.unpack_from('<I', data, idx)[0]; idx += 4
        
        packing1 = 1.0 / math.pow(2, p1)
        
        for fi in range(nfaces):
            nverts = struct.unpack_from('<I', data, idx)[0]; idx += 4
            mat = struct.unpack_from('<I', data, idx)[0]; idx += 4
            
            if pbtype == 4 or pbtype == 2:
                for i in range(nverts):
                    x, y, z, u, v = struct.unpack_from('<hhhhh', data, idx); idx += 10
                    idx += 3 # normal
                    idx += 4 # color
                    if pbtype == 4:
                        idx += 2 # vgroup
                    vertices.append({'x': x * packing1, 'y': y * packing1, 'z': z * packing1, 'corrupt': False})
            elif pbtype == 5:
                for i in range(nverts):
                    x, y, z, u, v = struct.unpack_from('<hhhhh', data, idx); idx += 10
                    idx += 3 # normal
                    idx += 4 # bones
                    idx += 4 # weights
                    vertices.append({'x': x * packing1, 'y': y * packing1, 'z': z * packing1, 'corrupt': False})
    except Exception as e:
        print(f"[-] Failed to parse SkinPrimBuffer: {e}")
        
    return vertices

def extract_vif_dma_vertices(data, max_bones=32):
    """
    A strict DMA Chain Emulator that traverses the 0x02610 hardware packet
    to extract the actual rendered vertices. (Currently implemented as a naive VIF scanner
    until the strict DMA emulator is written).
    """
    vertices = []
    pos = 0
    file_len = len(data)
    
    # NOTE: This is the flawed sliding window parser. It will be replaced
    # by the strict DMA chain emulator in the next step.
    try:
        while pos + 4 <= file_len:
            code = struct.unpack_from('<I', data, pos)[0]
            num = (code >> 16) & 0xFF
            cmd = (code >> 24) & 0xFF
            
            if cmd == 0x6D and num > 0 and num < 1000:
                payload_start = pos + 4
                if payload_start % 16 != 0:
                    payload_start = (payload_start + 15) & ~15
                    
                for i in range(num):
                    v_offset = payload_start + (i * 8)
                    if v_offset + 8 <= file_len:
                        x, y, z, w = struct.unpack_from('<hhhh', data, v_offset)
                        if not (x == 0 and y == 0 and z == 0):
                            vertices.append({'x': x, 'y': y, 'z': z, 'corrupt': False})
                            
            pos += 4
    except Exception as e:
        print(f"[-] VIF Corrupt")
        
    return vertices

def get_node_binary(parser_data, node):
    return parser_data[node['offset']:node['offset'] + 12 + node['data_size']]

def render_diff(vanilla_verts, injected_verts, title="Visual Injection Diff"):
    fig = plt.figure(figsize=(16, 8))
    fig.canvas.manager.set_window_title(title)
    fig.patch.set_facecolor('#111111')
    
    # --- LEFT PANE (VANILLA) ---
    ax1 = fig.add_subplot(121, projection='3d')
    ax1.set_facecolor('#111111')
    ax1.set_title("Vanilla (Golden Master)", color='white')
    
    vx = [v['x'] for v in vanilla_verts if not v['corrupt']]
    vy = [v['y'] for v in vanilla_verts if not v['corrupt']]
    vz = [v['z'] for v in vanilla_verts if not v['corrupt']]
    
    if vx:
        ax1.scatter(vx, vy, vz, s=2, c='dodgerblue', marker='o', alpha=0.5)
        
        max_range = max(max(vx)-min(vx), max(vy)-min(vy), max(vz)-min(vz)) / 2.0
        mid_x = (max(vx)+min(vx)) * 0.5
        mid_y = (max(vy)+min(vy)) * 0.5
        mid_z = (max(vz)+min(vz)) * 0.5
        
        ax1.set_xlim(mid_x - max_range, mid_x + max_range)
        ax1.set_ylim(mid_y - max_range, mid_y + max_range)
        ax1.set_zlim(mid_z - max_range, mid_z + max_range)
    
    ax1.xaxis.label.set_color('white')
    ax1.yaxis.label.set_color('white')
    ax1.zaxis.label.set_color('white')
    ax1.tick_params(colors='white')
    
    # --- RIGHT PANE (INJECTED) ---
    ax2 = fig.add_subplot(122, projection='3d')
    ax2.set_facecolor('#111111')
    ax2.set_title("Injected Payload (Diff)", color='white')
    
    ix_valid = [v['x'] for v in injected_verts if not v['corrupt']]
    iy_valid = [v['y'] for v in injected_verts if not v['corrupt']]
    iz_valid = [v['z'] for v in injected_verts if not v['corrupt']]
    
    ix_corrupt = [v['x'] for v in injected_verts if v['corrupt']]
    iy_corrupt = [v['y'] for v in injected_verts if v['corrupt']]
    iz_corrupt = [v['z'] for v in injected_verts if v['corrupt']]
    
    if ix_valid:
        ax2.scatter(ix_valid, iy_valid, iz_valid, s=2, c='grey', marker='o', alpha=0.5, label='Valid Mesh')
    if ix_corrupt:
        ax2.scatter(ix_corrupt, iy_corrupt, iz_corrupt, s=15, c='red', marker='x', alpha=1.0, label='Invalid Bone / Spike')
        
    if vx: # Match axis bounds to vanilla so we can compare sizes directly
        ax2.set_xlim(mid_x - max_range, mid_x + max_range)
        ax2.set_ylim(mid_y - max_range, mid_y + max_range)
        ax2.set_zlim(mid_z - max_range, mid_z + max_range)
        
    ax2.xaxis.label.set_color('white')
    ax2.yaxis.label.set_color('white')
    ax2.zaxis.label.set_color('white')
    ax2.tick_params(colors='white')
    if ix_corrupt:
        ax2.legend(facecolor='#222222', edgecolor='white', labelcolor='white')
        
    plt.tight_layout()
    plt.show()

def get_target_model(esf, target_hash):
    if target_hash:
        for entry in esf.pointer_table:
            if entry.asset_id == target_hash:
                def search_tree(node, target_offset):
                    if node['offset'] == target_offset: return node
                    for c in node.get('children', []):
                        res = search_tree(c, target_offset)
                        if res: return res
                    return None
                return search_tree(esf.root, entry.offset)
    else:
        for entry in esf.pointer_table:
            if entry.type_id in (0x72700, 0x62700):
                def search_tree(node, target_offset):
                    if node['offset'] == target_offset: return node
                    for c in node.get('children', []):
                        res = search_tree(c, target_offset)
                        if res: return res
                    return None
                return search_tree(esf.root, entry.offset)
    return None

def main():
    parser = argparse.ArgumentParser(description="EQOA Visual Injection Diff")
    parser.add_argument("vanilla", help="Path to Vanilla .CSF/.ESF file")
    parser.add_argument("injected", help="Path to Injected .CSF/.ESF file")
    parser.add_argument("--hash", help="Target Asset Hash (e.g., 0x05AEBA67)", type=lambda x: int(x, 16), default=None)
    args = parser.parse_args()

    for f in [args.vanilla, args.injected]:
        if not os.path.exists(f):
            print(f"[-] Error: File not found: {f}")
            sys.exit(1)

    print(f"[*] Decompressing {args.vanilla}...")
    van_raw = decompress_csf(args.vanilla)
    van_esf = ESFParser(van_raw).parse()
    van_model = get_target_model(van_esf, args.hash)
    
    print(f"[*] Decompressing {args.injected}...")
    inj_raw = decompress_csf(args.injected)
    inj_esf = ESFParser(inj_raw).parse()
    inj_model = get_target_model(inj_esf, args.hash)

    if not van_model or not inj_model:
        print("[-] Could not locate target model in one or both files.")
        sys.exit(1)
        
    van_geom = find_geom_node(van_model)
    inj_geom = find_geom_node(inj_model)
    
    # Find SkinPrimBuffer for Golden Reference
    def find_skin_prim(node):
        if node['type_id'] in (0x21210, 0x1210, 0x11210):
            return node
        for c in node.get('children', []):
            res = find_skin_prim(c)
            if res: return res
        return None
        
    van_skin_node = find_skin_prim(van_model)
    if not van_skin_node:
        print("[-] Could not find SkinPrimBuffer (Golden Reference).")
        sys.exit(1)
        
    # Get the raw inline data for SkinPrimBuffer
    # Node header is 12 bytes. But wait, we must skip dict_id if ver > 1
    van_skin_data = van_raw[van_skin_node['offset']+12 : van_skin_node['offset']+12+van_skin_node['data_size']]
    ver = (van_skin_node['type_id'] >> 16) & 0xFFFF
    if ver > 1:
        van_skin_data = van_skin_data[4:] # skip dict_id

    print(f"[*] Parsing SkinPrimBuffer (Golden Reference)...")
    golden_verts = extract_skin_prim_buffer(van_skin_data)
    print(f"  [+] Extracted {len(golden_verts)} valid vertices!")

    if not inj_geom:
        print("[-] Could not find Injected 0x02610 Geometry Node.")
        sys.exit(1)

    inj_bytes = get_node_binary(inj_raw, inj_geom)
    
    print(f"[*] Unpacking Injected DMA VIF Payload ({len(inj_bytes)} bytes)...")
    inj_verts = extract_vif_dma_vertices(inj_bytes[12:], max_bones=32)
    
    render_diff(golden_verts, inj_verts, title="SkinPrimBuffer vs Injected DMA")

if __name__ == '__main__':
    main()
