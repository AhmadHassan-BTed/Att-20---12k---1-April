import os, struct
from esf_parser import ESFParser

def parse_node(data, pos):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node = {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'children': [],
        'inline_data': None
    }
    
    next_pos = pos + 12
    if child_count == 0:
        node['inline_data'] = data[next_pos:next_pos+data_size]
        next_pos += data_size
    else:
        for _ in range(child_count):
            child, next_pos = parse_node(data, next_pos)
            node['children'].append(child)
            
    return node, next_pos

print("[*] Parsing Expansion/Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()
exp_hashes = {entry.asset_id for entry in exp_parser.pointer_table if entry.asset_id is not None}
print(f"[+] Frontiers has {len(exp_hashes)} unique asset hashes in CHAR.ESF.")

# Read a Vanilla payload
payload_path = 'workspace/payloads/asset_0x2EF8E480.bin'
print(f"\n[*] Parsing Vanilla model: {payload_path}")
with open(payload_path, 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

# Helper to find all texture/palette leaf nodes (type 0x01000)
def find_leaves(node, type_id, lst):
    if node['child_count'] == 0:
        if node['type_id'] == type_id:
            lst.append(node)
    for child in node['children']:
        find_leaves(child, type_id, lst)

van_textures = []
find_leaves(van_node, 0x01000, van_textures)
print(f"[+] Vanilla model has {len(van_textures)} embedded texture/palette data nodes (type 0x01000).")

# Wait! Let's check child [1] which is 0x11110 (contains textures/palettes)
# Let's inspect its children to see if there are any other referenced asset hashes!
van_11110 = van_node['children'][1]
print(f"Vanilla child 1 structure: type=0x{van_11110['type_id']:05X}, children={van_11110['child_count']}")
for i, child in enumerate(van_11110['children']):
    print(f"  [{i}]: type=0x{child['type_id']:05X}, children={child['child_count']}, size={child['data_size']}")
