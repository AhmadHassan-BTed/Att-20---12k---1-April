import os
import sys
import struct
from collections import defaultdict

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def get_node_binary(parser_data, node):
    return parser_data[node['offset']:node['offset'] + 12 + node['data_size']]

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
    
    tallies = defaultdict(int)
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', geom_data, pos)[0]
        cmd = (code >> 24) & 0xFF
        num = (code >> 16) & 0xFF
        imm = code & 0xFFFF
        
        # We only want to tally commands that we think we recognize
        if 0x00 <= cmd <= 0x7F:
            tallies[cmd] += 1
            
        pos += 4
        
    for cmd, count in sorted(tallies.items()):
        print(f"CMD 0x{cmd:02X}: {count} occurrences")

if __name__ == "__main__":
    debug()
