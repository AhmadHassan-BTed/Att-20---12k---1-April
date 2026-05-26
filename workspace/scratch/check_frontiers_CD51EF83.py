import os
import sys
sys.path.append('core')
from esf_parser import ESFParser
from verify_injected_models import parse_node

def main():
    frontiers_esf = 'workspace/expansion/CHAR.ESF'
    with open(frontiers_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    node_bytes = data[entry.offset : entry.offset + entry.length]
    root, _ = parse_node(node_bytes, 0)
    
    b070 = root['children'][3]
    print(f"Frontiers 0x0B070 size: {b070['data_size']}, children count: {len(b070['children'])}")
    for idx, child in enumerate(b070['children']):
        print(f"  Child {idx}: type=0x{child['type_id']:X}, size={child['data_size']}")
        for s_idx, sub in enumerate(child['children']):
            print(f"    Sub {s_idx}: type=0x{sub['type_id']:X}, size={sub['data_size']}")

if __name__ == '__main__':
    main()
