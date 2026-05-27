#!/usr/bin/env python3
"""
offline_vif_viewer.py
=====================
Parses 0x6C (V4-32) VIF unpack arrays from PS2 .bin payloads and renders
the raw geometry in an interactive 3D scatter plot. 
This bypasses PCSX2 to provide an offline, scientific validation of PS2 meshes.
"""

import os
import sys
import struct
import math
try:
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
except ImportError:
    print("[-] Error: matplotlib is required. Run 'pip install matplotlib' first.")
    sys.exit(1)

def parse_vif_code(code_32bit):
    imm = code_32bit & 0xFFFF
    num = (code_32bit >> 16) & 0xFF
    cmd = (code_32bit >> 24) & 0xFF
    return {'imm': imm, 'num': num, 'cmd': cmd}

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
        vif = parse_vif_code(code)
        
        # 0x6C is V4-32 (Vertices). It unpacks 16 bytes per element.
        if vif['cmd'] == 0x6C and vif['num'] > 0:
            align_offset = (pos + 15) & ~15
            num = vif['num']
            
            for i in range(num):
                v_offset = align_offset + (i * 16)
                if v_offset + 12 <= file_len:
                    x, y, z = struct.unpack_from('<fff', data, v_offset)
                    
                    # Heuristics to filter out obvious garbage or bit-packed flags
                    if not math.isnan(x) and not math.isnan(y) and not math.isnan(z):
                        if abs(x) < 10000.0 and abs(y) < 10000.0 and abs(z) < 10000.0:
                            # Skip exact 0,0,0 collapses if we want to see the real mesh
                            if abs(x) > 0.00001 or abs(y) > 0.00001 or abs(z) > 0.00001:
                                vertices.append((x, y, z))
                                
            # Skip past the unpacked data to find the next VIF command
            pos = align_offset + (num * 16) - 4
            
        pos += 4
        
    return vertices

def plot_vertices(vertices, title="PS2 VIF Geometry"):
    if not vertices:
        print("[-] No valid V4-32 geometry found to plot.")
        return

    print(f"[*] Rendering {len(vertices)} valid vertices...")
    
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]
    
    # PS2 coordinate systems often have inverted axes, we plot raw for analysis
    ax.scatter(xs, ys, zs, s=2, c='cyan', marker='o', alpha=0.6, edgecolors='none')
    
    ax.set_xlabel('X Axis')
    ax.set_ylabel('Y Axis')
    ax.set_zlabel('Z Axis')
    ax.set_title(title)
    
    # Force proportional axes so the character doesn't look stretched
    max_range = max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs)) / 2.0
    mid_x = (max(xs)+min(xs)) * 0.5
    mid_y = (max(ys)+min(ys)) * 0.5
    mid_z = (max(zs)+min(zs)) * 0.5
    
    ax.set_xlim(mid_x - max_range, mid_x + max_range)
    ax.set_ylim(mid_y - max_range, mid_y + max_range)
    ax.set_zlim(mid_z - max_range, mid_z + max_range)
    
    # Dark background for cool PS2 wireframe aesthetic
    ax.set_facecolor('black')
    fig.patch.set_facecolor('black')
    ax.xaxis.label.set_color('white')
    ax.yaxis.label.set_color('white')
    ax.zaxis.label.set_color('white')
    ax.title.set_color('white')
    ax.tick_params(colors='white')
    ax.grid(True, color='gray', linestyle='--', linewidth=0.5, alpha=0.3)
    
    out_path = filepath.replace(".bin", "_plot.png")
    plt.savefig(out_path, dpi=150, bbox_inches='tight')
    print(f"[*] Plot saved to {out_path}")
    plt.close(fig)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python offline_vif_viewer.py <path_to_payload.bin>")
        sys.exit(1)
        
    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f"[-] Error: File not found -> {filepath}")
        sys.exit(1)
        
    print(f"[*] Parsing {os.path.basename(filepath)} for V4-32 Vertices...")
    with open(filepath, 'rb') as f:
        data = f.read()
        
    verts = extract_v4_32_vertices(data)
    plot_vertices(verts, title=f"VIF Geometry: {os.path.basename(filepath)}")
