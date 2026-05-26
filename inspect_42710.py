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

van_42710 = van_node['children'][0]
fro_42710 = fro_node['children'][0]

def dump_42710(node, label):
    print(f"\n=== {label} ===")
    data = node['inline_data']
    print("Hex:   ", data.hex())
    # Unpack as floats and ints
    floats = struct.unpack('<12f', data)
    ints = struct.unpack('<12I', data)
    print("Floats:", [f"{f:.4f}" for f in floats[:7]])
    print("Ints:  ", [f"0x{i:08X}" for i in ints])

dump_42710(van_42710, "Vanilla 0x42710")
dump_42710(fro_42710, "Frontiers 0x42710")
