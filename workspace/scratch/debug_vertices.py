import os
import sys
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def get_node_binary(parser_data, node):
    return parser_data[node['offset']:node['offset'] + 12 + node['data_size']]

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
            geom_data = get_node_binary(data, geom)
            break

    pos = 0
    file_len = len(geom_data)
    
    print("X\tY\tZ\tW (Bone?)\tCMD")
    count = 0
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', geom_data, pos)[0]
        num = (code >> 16) & 0xFF
        cmd = (code >> 24) & 0xFF
        
        if 0x60 <= cmd <= 0x6F and num > 0:
            vn = (cmd >> 2) & 3
            vl = cmd & 3
            if vl == 3: bytes_per_struct = 2
            else: bytes_per_struct = (vn + 1) * [4, 2, 1][vl]
            
            payload_size = num * bytes_per_struct
            padded_size = (payload_size + 3) & ~3
            
            payload_start = pos + 4
            qwc_payload_start = (payload_start + 15) & ~15
            
            if qwc_payload_start != payload_start:
                pad_val = struct.unpack_from('<I', geom_data, payload_start)[0]
                if pad_val == 0x00000000:
                    payload_start = qwc_payload_start
            
            if cmd in (0x6A, 0x6D) and num > 10:
                is_16 = (cmd == 0x6D)
                stride = 8 if is_16 else 4
                fmt = '<hhhh' if is_16 else '<bbbb'
                for i in range(min(5, num)):
                    v_offset = payload_start + (i * stride)
                    if v_offset + stride <= file_len:
                        x, y, z, w = struct.unpack_from(fmt, geom_data, v_offset)
                        print(f"{x}\t{y}\t{z}\t{w}\t{hex(cmd)}")
                count += 1
                if count >= 10: return
                        
            pos = payload_start + padded_size - 4
        pos += 4

if __name__ == "__main__":
    debug()
