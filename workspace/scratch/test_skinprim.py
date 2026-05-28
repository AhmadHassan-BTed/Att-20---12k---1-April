import os
import sys
import struct
import math

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def get_nodes(node, type_id, out_list):
    if node['type_id'] == type_id:
        out_list.append(node)
    for c in node.get('children', []):
        get_nodes(c, type_id, out_list)

def debug():
    with open("workspace/original/CHAR.ESF", 'rb') as f:
        data = f.read()
    esf = ESFParser(data).parse()
    
    for entry in esf.pointer_table:
        if entry.asset_id == 0x05AEBA67:
            def search_tree(node, target_offset):
                if node['offset'] == target_offset: return node
                for c in node.get('children', []):
                    res = search_tree(c, target_offset)
                    if res: return res
                return None
            
            model = search_tree(esf.root, entry.offset)
            n_1210 = []
            # In CHAR.ESF, SkinPrimBuffer is 0x21210 or 0x1210
            get_nodes(model, 0x21210, n_1210)
            
            if not n_1210:
                print("Could not find SkinPrimBuffer")
                return
                
            node = n_1210[0]
            pos = node['offset'] + 12
            inline_data = data[pos : pos + node['data_size']]
            
            idx = 0
            ver = (node['type_id'] >> 16) & 0xFFFF
            if ver > 1:
                dict_id = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
                
            pbtype = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            nmats = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            nfaces = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            unknown = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            p1 = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            p2 = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            p3 = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
            
            packing1 = 1.0 / math.pow(2, p1)
            
            print(f"SkinPrimBuffer:")
            print(f"  pbtype: {pbtype}")
            print(f"  nmats: {nmats}")
            print(f"  nfaces: {nfaces}")
            print(f"  p1: {p1} (packing: {packing1})")
            
            vertices = []
            
            for fi in range(nfaces):
                nverts = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
                mat = struct.unpack_from('<I', inline_data, idx)[0]; idx += 4
                
                if pbtype == 4 or pbtype == 2:
                    for i in range(nverts):
                        x, y, z, u, v = struct.unpack_from('<hhhhh', inline_data, idx); idx += 10
                        idx += 3 # normal
                        idx += 4 # color
                        vgroup = 0
                        if pbtype == 4:
                            vgroup = struct.unpack_from('<h', inline_data, idx)[0]; idx += 2
                        vertices.append((x * packing1, y * packing1, z * packing1))
                elif pbtype == 5:
                    for i in range(nverts):
                        x, y, z, u, v = struct.unpack_from('<hhhhh', inline_data, idx); idx += 10
                        idx += 3 # normal
                        idx += 4 # color (bones)
                        idx += 4 # color (weights)
                        vertices.append((x * packing1, y * packing1, z * packing1))
            
            print(f"Extracted {len(vertices)} vertices successfully!")
            break

if __name__ == "__main__":
    debug()
