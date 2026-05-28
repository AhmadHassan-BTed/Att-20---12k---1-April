import os
import sys
import struct
import glob
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def parse_node(d, p):
    if p + 12 > len(d): return None, p
    tid = struct.unpack_from('<I', d, p)[0]
    sz  = struct.unpack_from('<I', d, p + 4)[0]
    cnt = struct.unpack_from('<I', d, p + 8)[0]
    node = {'type_id': tid, 'data_size': sz, 'child_count': cnt, 'children': [], 'inline_data': None}
    p += 12
    if cnt == 0:
        node['inline_data'] = d[p:p+sz]
        p += sz
    else:
        for _ in range(cnt):
            child, p = parse_node(d, p)
            if child: node['children'].append(child)
    return node, p

def find_model_node(parser, target_hash):
    for child in parser.root['children']:
        if child['type_id'] == 0x0A010:
            for model_node in child['children']:
                h = parser._find_hash_in_subtree(model_node)
                if h == target_hash:
                    return model_node
    return None

def find_node(node, type_id):
    if node['type_id'] == type_id:
        return node
    for child in node['children']:
        res = find_node(child, type_id)
        if res: return res
    return None

def analyze_w_components(data):
    w_vals = set()
    pos = 0
    file_len = len(data)
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', data, pos)[0]
        cmd = (code >> 24) & 0xFF
        num = (code >> 16) & 0xFF
        if cmd == 0x6C and num > 0:  # V4-32
            v_off = pos + 4
            expected = num * 16
            if v_off + expected <= file_len:
                for i in range(min(5, num)): # just look at first 5 per array
                    w = struct.unpack_from('<I', data, v_off + 12)[0]
                    w_vals.add(f"0x{w:08X}")
                    v_off += 16
                pos = (pos + 4 + expected + 3) & ~3
            else:
                pos += 4
        else:
            pos += 4
    return list(w_vals)

def main():
    target_hash = 0x05AEBA67
    original_esf = "workspace/expansion/CHAR.ESF"
    
    with open(original_esf, 'rb') as f:
        orig_data = f.read()
    orig_parser = ESFParser(orig_data).parse()
    
    orig_model = find_model_node(orig_parser, target_hash)
    orig_geom = find_node(orig_model, 0x02610)
    
    w_vals = set()
    def gather(node):
        if node['child_count'] == 0 and node['inline_data']:
            w_vals.update(analyze_w_components(node['inline_data']))
        for child in node['children']:
            gather(child)
            
    gather(orig_geom)
    
    print("GOLDEN MASTER W-COMPONENT VALUES:")
    for w in sorted(w_vals):
        print(f"  {w}")

if __name__ == "__main__":
    main()
