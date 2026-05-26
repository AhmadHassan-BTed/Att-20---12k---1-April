import os, struct
from esf_parser import ESFParser

def parse_node(data, pos, depth=0):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node = {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'offset': pos,
        'children': [],
        'inline_data': None
    }
    
    next_pos = pos + 12
    if child_count == 0:
        node['inline_data'] = data[next_pos:next_pos+data_size]
        next_pos += data_size
    else:
        for _ in range(child_count):
            child, next_pos = parse_node(data, next_pos, depth + 1)
            node['children'].append(child)
            
    return node, next_pos

print("[*] Parsing Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

# Find the first entry in Frontiers that is type 0x62700 natively
native_62700_entry = None
for entry in exp_parser.pointer_table:
    if entry.type_id == 0x62700:
        native_62700_entry = entry
        break

if not native_62700_entry:
    print("[-] Error: Could not find any native 0x62700 entry in Frontiers ESF!")
    exit(1)

print(f"\n[+] Found native Frontiers 0x62700 entry:")
print(f"    {native_62700_entry}")

native_model_bytes = exp_data[native_62700_entry.offset : native_62700_entry.offset + native_62700_entry.length]
native_node, _ = parse_node(native_model_bytes, 0)

print("\n=== Native Frontiers 0x62700 root children ===")
for i, child in enumerate(native_node['children']):
    print(f"  [{i}]: 0x{child['type_id']:05X} (children={child['child_count']}, size={child['data_size']})")
