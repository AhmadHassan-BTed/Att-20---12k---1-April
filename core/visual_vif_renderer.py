#!/usr/bin/env python3
"""
visual_vif_renderer.py
======================
Parses .CSF (Compressed ESF) and .ESF files to extract character models,
unpack V4-32 VIF packets, and visually render the 3D geometry along with 
bounding boxes and bounding spheres in an interactive matplotlib 3D window.
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

# Ensure core modules can be imported
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser


def decompress_csf(filepath):
    """Decompress a .CSF file into a raw ESF/FJBO buffer."""
    with open(filepath, 'rb') as f:
        data = f.read()

    if not data.startswith(b'CESF'):
        return data  # It's likely already an uncompressed ESF

    print("[*] Detected CESF magic. Decompressing zlib streams...")
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
            print(f"[-] Zlib decompression error at offset 0x{idx:02X}: {e}")
            break

    print(f"[+] CSF Decompressed successfully. Size: {len(out_buf):,} bytes")
    return bytes(out_buf)


def extract_bounding_volumes(node):
    """
    Extract bounding box and sphere from the root model node inline data.
    Typically found around offset 0x20 to 0x47 in 0x72700 / 0x62700 nodes.
    Layout: 
    - min X, Y, Z (float32)
    - max X, Y, Z (float32)
    - sphere X, Y, Z, R (float32)
    """
    bounds = {
        'min_x': 0.0, 'min_y': 0.0, 'min_z': 0.0,
        'max_x': 0.0, 'max_y': 0.0, 'max_z': 0.0,
        'sphere_x': 0.0, 'sphere_y': 0.0, 'sphere_z': 0.0, 'sphere_r': 0.0,
        'corrupted': False
    }
    
    inline = node.get('inline_data', b'')
    if len(inline) >= 0x48:
        try:
            # Typical float layout at offset 0x20
            minX, minY, minZ, maxX, maxY, maxZ = struct.unpack_from('<ffffff', inline, 0x20)
            sX, sY, sZ, sR = struct.unpack_from('<ffff', inline, 0x38)
            
            bounds['min_x'] = minX
            bounds['min_y'] = minY
            bounds['min_z'] = minZ
            bounds['max_x'] = maxX
            bounds['max_y'] = maxY
            bounds['max_z'] = maxZ
            
            bounds['sphere_x'] = sX
            bounds['sphere_y'] = sY
            bounds['sphere_z'] = sZ
            bounds['sphere_r'] = sR
            
            # Check for corruption (0 radius, exactly 0 on all bounds, or NaNs)
            if math.isnan(minX) or sR <= 0.0 or (minX == 0 and maxX == 0 and minY == 0):
                bounds['corrupted'] = True
                
        except struct.error:
            bounds['corrupted'] = True
    else:
        bounds['corrupted'] = True
        
    return bounds


def extract_v4_32_vertices(data):
    """
    Scans binary for V4-32 (0x6C) VIF unpack commands and extracts 
    the X, Y, Z float coordinates.
    """
    vertices = []
    pos = 0
    file_len = len(data)
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', data, pos)[0]
        imm = code & 0xFFFF
        num = (code >> 16) & 0xFF
        cmd = (code >> 24) & 0xFF
        
        # 0x6C is V4-32 (Vertices). It unpacks 16 bytes per element.
        if cmd == 0x6C and num > 0:
            align_offset = (pos + 15) & ~15
            
            for i in range(num):
                v_offset = align_offset + (i * 16)
                if v_offset + 12 <= file_len:
                    x, y, z = struct.unpack_from('<fff', data, v_offset)
                    
                    if not math.isnan(x) and not math.isnan(y) and not math.isnan(z):
                        if abs(x) < 10000.0 and abs(y) < 10000.0 and abs(z) < 10000.0:
                            if abs(x) > 0.00001 or abs(y) > 0.00001 or abs(z) > 0.00001:
                                vertices.append((x, y, z))
                                
            pos = align_offset + (num * 16) - 4
            
        pos += 4
        
    return vertices


def find_geom_node(node):
    if node['type_id'] == 0x02610:
        return node
    for child in node.get('children', []):
        found = find_geom_node(child)
        if found:
            return found
    return None

def get_node_binary(parser_data, node):
    return parser_data[node['offset']:node['offset'] + 12 + node['data_size']]

def render_3d_mesh(vertices, bounds, title):
    print(f"[*] Rendering {len(vertices)} vertices...")
    
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]
    
    # 1. Plot Vertices
    ax.scatter(xs, ys, zs, s=2, c='cyan', marker='o', alpha=0.6, edgecolors='none', label='V4-32 Vertices')
    
    # 2. Plot Bounding Box
    box_color = 'red' if bounds['corrupted'] else 'lime'
    if not bounds['corrupted'] or (bounds['max_x'] != 0.0):
        # Define the 8 corners of the bounding box
        corners = [
            (bounds['min_x'], bounds['min_y'], bounds['min_z']),
            (bounds['max_x'], bounds['min_y'], bounds['min_z']),
            (bounds['max_x'], bounds['max_y'], bounds['min_z']),
            (bounds['min_x'], bounds['max_y'], bounds['min_z']),
            (bounds['min_x'], bounds['min_y'], bounds['max_z']),
            (bounds['max_x'], bounds['min_y'], bounds['max_z']),
            (bounds['max_x'], bounds['max_y'], bounds['max_z']),
            (bounds['min_x'], bounds['max_y'], bounds['max_z'])
        ]
        
        # Edges connecting the corners
        edges = [
            (0,1), (1,2), (2,3), (3,0), # Bottom
            (4,5), (5,6), (6,7), (7,4), # Top
            (0,4), (1,5), (2,6), (3,7)  # Pillars
        ]
        
        for edge in edges:
            ax.plot3D(
                [corners[edge[0]][0], corners[edge[1]][0]],
                [corners[edge[0]][1], corners[edge[1]][1]],
                [corners[edge[0]][2], corners[edge[1]][2]],
                box_color, linewidth=1.5, alpha=0.8
            )
        
        # Custom proxy artist for legend
        ax.plot([], [], [], color=box_color, label='Bounding Box')
            
    # 3. Aesthetics
    ax.set_xlabel('X Axis')
    ax.set_ylabel('Y Axis')
    ax.set_zlabel('Z Axis')
    ax.set_title(title)
    
    # Proportional axes
    if xs:
        max_range = max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs)) / 2.0
        mid_x = (max(xs)+min(xs)) * 0.5
        mid_y = (max(ys)+min(ys)) * 0.5
        mid_z = (max(zs)+min(zs)) * 0.5
        
        ax.set_xlim(mid_x - max_range, mid_x + max_range)
        ax.set_ylim(mid_y - max_range, mid_y + max_range)
        ax.set_zlim(mid_z - max_range, mid_z + max_range)
    
    # Dark PS2 wireframe aesthetic
    ax.set_facecolor('#111111')
    fig.patch.set_facecolor('#111111')
    ax.xaxis.label.set_color('white')
    ax.yaxis.label.set_color('white')
    ax.zaxis.label.set_color('white')
    ax.title.set_color('white')
    ax.tick_params(colors='white')
    ax.grid(True, color='gray', linestyle='--', linewidth=0.5, alpha=0.3)
    ax.legend(facecolor='#222222', edgecolor='white', labelcolor='white')
    
    plt.show()

def main():
    parser = argparse.ArgumentParser(description="EQOA Visual VIF Renderer")
    parser.add_argument("file", help="Path to .CSF or .ESF file")
    parser.add_argument("--hash", help="Target Asset Hash (e.g., 0x05AEBA67)", type=lambda x: int(x, 16), default=None)
    args = parser.parse_args()

    if not os.path.exists(args.file):
        print(f"[-] Error: File not found: {args.file}")
        sys.exit(1)

    print(f"[*] Processing {args.file}")
    raw_esf = decompress_csf(args.file)
    
    print("[*] Parsing FJBO ESF Tree...")
    esf = ESFParser(raw_esf).parse()
    
    target_node = None
    if args.hash:
        print(f"[*] Searching for Target Hash: 0x{args.hash:08X}")
        for entry in esf.pointer_table:
            if entry.asset_id == args.hash:
                # Need to find this node in the tree
                def search_tree(node, target_offset):
                    if node['offset'] == target_offset: return node
                    for c in node.get('children', []):
                        res = search_tree(c, target_offset)
                        if res: return res
                    return None
                target_node = search_tree(esf.root, entry.offset)
                break
        if not target_node:
            print(f"[-] Could not find asset 0x{args.hash:08X} in file.")
            sys.exit(1)
    else:
        # Just grab the first Character Model node (0x72700 / 0x62700)
        for entry in esf.pointer_table:
            if entry.type_id in (0x72700, 0x62700):
                print(f"[*] Auto-selected first model: 0x{entry.asset_id:08X}")
                def search_tree(node, target_offset):
                    if node['offset'] == target_offset: return node
                    for c in node.get('children', []):
                        res = search_tree(c, target_offset)
                        if res: return res
                    return None
                target_node = search_tree(esf.root, entry.offset)
                args.hash = entry.asset_id
                break

    if not target_node:
        print("[-] No valid Character Models (0x72700 / 0x62700) found in file.")
        sys.exit(1)

    bounds = extract_bounding_volumes(target_node)
    print(f"[*] Bounding Box Extracted: Min({bounds['min_x']:.2f}, {bounds['min_y']:.2f}, {bounds['min_z']:.2f}) -> Max({bounds['max_x']:.2f}, {bounds['max_y']:.2f}, {bounds['max_z']:.2f})")
    if bounds['corrupted']:
        print("  [!] WARNING: Bounding Box is 0, NaN, or corrupted!")

    geom_node = find_geom_node(target_node)
    if not geom_node:
        print("[-] Could not find 0x02610 Geometry Node in model.")
        sys.exit(1)
        
    geom_bytes = get_node_binary(raw_esf, geom_node)
    print(f"[*] Extracted 0x02610 Geometry block ({len(geom_bytes):,} bytes)")
    
    verts = extract_v4_32_vertices(geom_bytes)
    
    hash_str = f"0x{args.hash:08X}" if args.hash else "Unknown"
    title = f"PS2 VIF Mesh: {hash_str} | Bounds Corrupted: {bounds['corrupted']}"
    
    render_3d_mesh(verts, bounds, title)

if __name__ == '__main__':
    main()
