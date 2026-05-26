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

# In our tree dump:
# Vanilla: child 2 is 0x05000 (size 120), child 7 is 0x05000 (size 424)
# Frontiers: child 2 is 0x05000 (size 120), child 7 is 0x05000 (size 352)

van_05000_c2 = van_node['children'][2]
van_05000_c7 = van_node['children'][7]

fro_05000_c2 = fro_node['children'][2]
fro_05000_c7 = fro_node['children'][7]

def dump_node_brief(node, label):
    print(f"\n=== {label} (size={len(node['inline_data'])} B) ===")
    data = node['inline_data']
    # Print as floats if possible
    floats = []
    for i in range(0, min(len(data), 64), 4):
        floats.append(struct.unpack_from('<f', data, i)[0])
    print("Floats:", [f"{f:.4f}" for f in floats])
    print("Hex:   ", data[:64].hex())

dump_node_brief(van_05000_c2, "Vanilla Child 2 (0x05000)")
dump_node_brief(fro_05000_c2, "Frontiers Child 2 (0x05000)")

dump_node_brief(van_05000_c7, "Vanilla Child 7 (0x05000)")
dump_node_brief(fro_05000_c7, "Frontiers Child 7 (0x05000)")
