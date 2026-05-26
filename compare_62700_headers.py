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

print("[*] Parsing original/Vanilla CHAR.ESF...")
with open('workspace/original/CHAR.ESF', 'rb') as f:
    orig_data = f.read()
orig_parser = ESFParser(orig_data).parse()

print("[*] Parsing expansion/Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

# Find 0x2EF8E480 in both
van_entry = None
for entry in orig_parser.pointer_table:
    if entry.asset_id == 0x2EF8E480:
        van_entry = entry
        break

fro_entry = None
for entry in exp_parser.pointer_table:
    if entry.asset_id == 0x2EF8E480:
        fro_entry = entry
        break

print(f"\nVanilla `0x2EF8E480` Entry:   {van_entry}")
print(f"Frontiers `0x2EF8E480` Entry: {fro_entry}")

van_model_bytes = orig_data[van_entry.offset : van_entry.offset + van_entry.length]
van_node, _ = parse_node(van_model_bytes, 0)

fro_model_bytes = exp_data[fro_entry.offset : fro_entry.offset + fro_entry.length]
fro_node, _ = parse_node(fro_model_bytes, 0)

print("\n=== Vanilla `0x2EF8E480` children ===")
for i, child in enumerate(van_node['children']):
    print(f"  [{i}]: 0x{child['type_id']:05X} (children={child['child_count']}, size={child['data_size']})")

print("\n=== Frontiers `0x2EF8E480` children ===")
for i, child in enumerate(fro_node['children']):
    print(f"  [{i}]: 0x{child['type_id']:05X} (children={child['child_count']}, size={child['data_size']})")
