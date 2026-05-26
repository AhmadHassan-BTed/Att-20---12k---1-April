import os
import sys
import struct
sys.path.append('core')
from esf_parser import ESFParser
from verify_injected_models import parse_node

def main():
    merged_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    with open(merged_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    node_bytes = data[entry.offset : entry.offset + entry.length]
    root, _ = parse_node(node_bytes, 0)
    
    b070 = root['children'][3]
    print(f"Rebuilt 0x0B070 children absolute offsets in file:")
    
    def get_child_offset(parent_bytes, parent_node, target_type):
        pos = 12
        for idx, child in enumerate(parent_node['children']):
            if child['type_id'] == target_type:
                return pos
            pos += 12 + child['data_size']
        return None
        
    b070_rel = get_child_offset(node_bytes, root, 0x0B070)
    print(f"0x0B070 starts at relative offset: 0x{b070_rel:X}")
    
    # Inside 0x0B070:
    pos = 12
    for idx, child in enumerate(b070['children']):
        print(f"  Child {idx} (type=0x{child['type_id']:X}): starts at relative 0x{pos:X} (absolute 0x{entry.offset + b070_rel + pos:X}), size={child['data_size']}")
        pos += 12 + child['data_size']

if __name__ == '__main__':
    main()
