import struct

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

with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

van_child6 = van_node['children'][6]
fro_child6 = fro_node['children'][6]

print(f"Vanilla Child 6: type=0x{van_child6['type_id']:05X}, size={len(van_child6['inline_data'])} B")
print(f"  Start hex: {van_child6['inline_data'][:64].hex()}")
print(f"  End hex:   {van_child6['inline_data'][-64:].hex()}")

print(f"\nFrontiers Child 6: type=0x{fro_child6['type_id']:05X}, size={len(fro_child6['inline_data'])} B")
print(f"  Start hex: {fro_child6['inline_data'][:64].hex()}")
print(f"  End hex:   {fro_child6['inline_data'][-64:].hex()}")
