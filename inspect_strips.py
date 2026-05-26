import struct

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

with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

# Find 0x02610 node in both
van_02610 = [c for c in van_node['children'] if c['type_id'] == 0x02610][0]
fro_02610 = [c for c in fro_node['children'] if c['type_id'] == 0x02610][0]

print("=== Vanilla 0x32600 first 3 strips ===")
for i in range(3):
    strip = van_02610['children'][i]
    print(f"Strip {i}: size={len(strip['inline_data'])} B")
    print(f"  Hex: {strip['inline_data'][:64].hex()}")

print("\n=== Frontiers 0x32600 first 3 strips ===")
for i in range(3):
    strip = fro_02610['children'][i]
    print(f"Strip {i}: size={len(strip['inline_data'])} B")
    print(f"  Hex: {strip['inline_data'][:64].hex()}")
