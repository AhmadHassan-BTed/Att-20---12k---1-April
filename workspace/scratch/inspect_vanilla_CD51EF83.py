import os
import sys
sys.path.append('core')
from esf_parser import ESFParser
from verify_injected_models import parse_node

def main():
    original_esf = 'workspace/original/CHAR.ESF'
    if not os.path.exists(original_esf):
        print("Original Vanilla CHAR.ESF not found!")
        return
        
    with open(original_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    node_bytes = data[entry.offset : entry.offset + entry.length]
    root, _ = parse_node(node_bytes, 0)
    
    print(f"Vanilla root children count: {len(root['children'])}")
    for idx, child in enumerate(root['children']):
        print(f"Child {idx}: type=0x{child['type_id']:05X}, size={child['data_size']}, children={child['child_count']}")

if __name__ == '__main__':
    main()
