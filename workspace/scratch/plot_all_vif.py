import os
import sys
import struct
import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def debug():
    with open("workspace/original/CHAR.ESF", 'rb') as f:
        data = f.read()
    esf = ESFParser(data).parse()
    
    geom_offset = None
    geom_size = None
    for entry in esf.pointer_table:
        if entry.asset_id == 0x05AEBA67:
            def search_tree(node, target_offset):
                if node['offset'] == target_offset: return node
                for c in node.get('children', []):
                    res = search_tree(c, target_offset)
                    if res: return res
                return None
            def find_geom(node):
                if node['type_id'] == 0x02610: return node
                for c in node.get('children', []):
                    res = find_geom(c)
                    if res: return res
                return None
            model = search_tree(esf.root, entry.offset)
            geom = find_geom(model)
            geom_data = data[geom['offset']:geom['offset'] + 12 + geom['data_size']]
            break

    file_len = len(geom_data)
    
    # Store points for each cmd
    cmd_points = {cmd: [] for cmd in range(0x60, 0x70)}
    
    pos = 0
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', geom_data, pos)[0]
        num = (code >> 16) & 0xFF
        cmd = (code >> 24) & 0xFF
        
        if 0x60 <= cmd <= 0x6F and num > 0:
            vn = (cmd >> 2) & 3
            vl = cmd & 3
            if vl == 3:
                bytes_per_struct = 2
            else:
                bytes_per_struct = (vn + 1) * [4, 2, 1][vl]
                
            payload_size = num * bytes_per_struct
            padded_size = (payload_size + 3) & ~3
            
            payload_start = pos + 4
            
            # Use raw extraction without guessing alignment
            # Just extract as floats or ints
            for i in range(num):
                v_offset = payload_start + (i * bytes_per_struct)
                if v_offset + bytes_per_struct <= file_len:
                    x, y, z = 0.0, 0.0, 0.0
                    
                    if vl == 0: # 32-bit floats
                        if vn >= 0: x = struct.unpack_from('<f', geom_data, v_offset)[0]
                        if vn >= 1: y = struct.unpack_from('<f', geom_data, v_offset + 4)[0]
                        if vn >= 2: z = struct.unpack_from('<f', geom_data, v_offset + 8)[0]
                    elif vl == 1: # 16-bit ints
                        if vn >= 0: x = float(struct.unpack_from('<h', geom_data, v_offset)[0])
                        if vn >= 1: y = float(struct.unpack_from('<h', geom_data, v_offset + 2)[0])
                        if vn >= 2: z = float(struct.unpack_from('<h', geom_data, v_offset + 4)[0])
                    elif vl == 2: # 8-bit ints
                        if vn >= 0: x = float(struct.unpack_from('<b', geom_data, v_offset)[0])
                        if vn >= 1: y = float(struct.unpack_from('<b', geom_data, v_offset + 1)[0])
                        if vn >= 2: z = float(struct.unpack_from('<b', geom_data, v_offset + 2)[0])
                        
                    if not math.isnan(x) and not math.isnan(y) and not math.isnan(z):
                        cmd_points[cmd].append((x, y, z))
                        
            pos = payload_start + padded_size - 4
        pos += 4

    # Now plot each cmd to a PNG
    artifact_dir = r"C:\Users\PMLS\.gemini\antigravity-ide\brain\93802275-4972-48b4-a5a3-466577a3d03f\scratch"
    os.makedirs(artifact_dir, exist_ok=True)
    
    for cmd, pts in cmd_points.items():
        if len(pts) > 50:
            fig = plt.figure(figsize=(8, 8))
            ax = fig.add_subplot(111, projection='3d')
            ax.set_facecolor('#111')
            fig.patch.set_facecolor('#111')
            
            xs = [p[0] for p in pts]
            ys = [p[1] for p in pts]
            zs = [p[2] for p in pts]
            
            ax.scatter(xs, ys, zs, s=2, c='cyan', marker='o', alpha=0.5)
            ax.set_title(f"CMD {hex(cmd)} - {len(pts)} pts", color='white')
            
            ax.xaxis.label.set_color('white')
            ax.yaxis.label.set_color('white')
            ax.zaxis.label.set_color('white')
            ax.tick_params(colors='white')
            
            out_file = os.path.join(artifact_dir, f"vif_{hex(cmd)}.png")
            plt.savefig(out_file, facecolor='#111', bbox_inches='tight')
            plt.close(fig)
            print(f"Saved {out_file}")

if __name__ == "__main__":
    debug()
