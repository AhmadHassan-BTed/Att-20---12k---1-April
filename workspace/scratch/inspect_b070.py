import os
import sys
import struct
from core.esf_parser import ESFParser
from core.verify_injected_models import parse_node

def main():
    merged_esf = "workspace/FINAL_CHAR_MERGED.ESF"
    if not os.path.exists(merged_esf):
        print("[-] Merged ESF not found.")
        return
        
    with open(merged_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    
    node_bytes = data[entry.offset : entry.offset + entry.length]
    root, _ = parse_node(node_bytes, 0)
    
    b070 = root['children'][3]
    print(f"0x0B070 Container: type=0x{b070['type_id']:05X}, size={b070['data_size']}, children={len(b070['children'])}")
    for idx, child in enumerate(b070['children']):
        print(f"  Child {idx}: type=0x{child['type_id']:05X}, size={child['data_size']}, children={child['child_count']}")
        if child['children']:
            for s_idx, sub in enumerate(child['children']):
                print(f"    Sub {s_idx}: type=0x{sub['type_id']:05X}, size={sub['data_size']}, children={sub['child_count']}")

if __name__ == '__main__':
    main()
