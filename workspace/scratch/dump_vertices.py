import os
import sys
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser
import zlib

def dump_verts():
    with open("workspace/original/CHAR.ESF", 'rb') as f:
        data = f.read()
    
    esf = ESFParser(data).parse()
    
    geom_offset = None
    geom_size = None
    for entry in esf.pointer_table:
        if entry.asset_id == 0x05AEBA67:
            # Found the model
            def find_geom(node):
                if node['type_id'] == 0x02610:
                    return node['offset'], node['data_size']
                for c in node.get('children', []):
                    res = find_geom(c)
                    if res: return res
                return None
                
            def search_tree(node, target_offset):
                if node['offset'] == target_offset: return node
                for c in node.get('children', []):
                    res = search_tree(c, target_offset)
                    if res: return res
                return None
                
            model_node = search_tree(esf.root, entry.offset)
            res = find_geom(model_node)
            if res:
                geom_offset, geom_size = res
            break
            
    if not geom_offset:
        print("Geom not found")
        return
        
    geom_data = data[geom_offset : geom_offset + 12 + geom_size]
    
    pos = 0
    count = 0
    print("X\t\tY\t\tZ\t\tW (Float)\tW (Int/Hex)\tW (Int)")
    while pos + 4 <= len(geom_data):
        code = struct.unpack_from('<I', geom_data, pos)[0]
        num = (code >> 16) & 0xFF
        cmd = (code >> 24) & 0xFF
        
        if cmd == 0x6C and num > 0:
            align_offset = (pos + 15) & ~15
            for i in range(num):
                v_offset = align_offset + (i * 16)
                if v_offset + 16 <= len(geom_data):
                    x, y, z, w = struct.unpack_from('<ffff', geom_data, v_offset)
                    w_int = struct.unpack_from('<I', geom_data, v_offset + 12)[0]
                    print(f"{x:10.4f}\t{y:10.4f}\t{z:10.4f}\t{w:10.4f}\t0x{w_int:08X}\t{w_int}")
                    count += 1
                    if count >= 30:
                        return
            pos = align_offset + (num * 16) - 4
        pos += 4

if __name__ == "__main__":
    dump_verts()
