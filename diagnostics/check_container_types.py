import os
from esf_parser import ESFParser
from collections import Counter

print("[*] Parsing Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    data = f.read()

parser = ESFParser(data).parse()

print(f"\nRoot node children count: {len(parser.root['children'])}")
for i, container in enumerate(parser.root['children']):
    c_type = container['type_id']
    c_children = container.get('children', [])
    print(f"\nContainer [{i}]: type=0x{c_type:05X}, children={len(c_children)}, size={container['data_size']:,} B")
    
    # Count types of children inside this container
    child_types = Counter(child['type_id'] for child in c_children)
    for tid, cnt in sorted(child_types.items(), key=lambda x: x[1], reverse=True):
        print(f"  - Child type 0x{tid:05X}: {cnt} entries")
