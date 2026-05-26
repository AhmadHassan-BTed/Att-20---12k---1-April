import os
import sys
import struct
sys.path.append('core')
from esf_parser import ESFParser
from verify_injected_models import parse_node

def main():
    original_esf = 'workspace/original/CHAR.ESF'
    with open(original_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    node_bytes = data[entry.offset : entry.offset + entry.length]
    root, _ = parse_node(node_bytes, 0)
    
    b070 = root['children'][3]
    child3 = b070['children'][3] # Child 3 is index 3
    sub1 = child3['children'][1] # Sub 1 is index 1
    
    print(f"Vanilla Child 3 Sub 1 type: 0x{sub1['type_id']:X}")
    print(f"Vanilla Child 3 Sub 1 data_size: {sub1['data_size']}")
    
    inline_data = sub1['inline_data']
    print(f"Tail of original Vanilla inline_data (last 64 bytes):")
    for i in range(0, len(inline_data[-64:]), 16):
        chunk = inline_data[-64:][i:i+16]
        hex_str = " ".join(f"{b:02X}" for b in chunk)
        print(f"  {len(inline_data) - 64 + i:04X}: {hex_str}")

if __name__ == '__main__':
    main()
