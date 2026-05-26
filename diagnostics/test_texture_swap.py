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

def update_node_sizes(node):
    if node['child_count'] == 0:
        node['data_size'] = len(node['inline_data'])
    else:
        size = 0
        for child in node['children']:
            update_node_sizes(child)
            size += 12 + child['data_size']
        node['data_size'] = size

def serialize_node(node):
    data = bytearray()
    header = struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    data.extend(header)
    
    if node['child_count'] == 0:
        if node['inline_data'] is not None:
            data.extend(node['inline_data'])
    else:
        for child in node['children']:
            data.extend(serialize_node(child))
    return bytes(data)

# Load Vanilla 0x2EF8E480
with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

# Load Frontiers 0x2EF8E480
with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

# Let's locate the texture container node (type 0x11110) in both
van_texture_node = [c for c in van_node['children'] if c['type_id'] == 0x11110][0]
fro_texture_idx = [i for i, c in enumerate(fro_node['children']) if c['type_id'] == 0x11110][0]

print(f"Vanilla texture node size: {van_texture_node['data_size']} bytes")
print(f"Frontiers original texture node size: {fro_node['children'][fro_texture_idx]['data_size']} bytes")

# Replace child in Frontiers with the Vanilla one
fro_node['children'][fro_texture_idx] = van_texture_node

# Recalculate sizes
update_node_sizes(fro_node)

# Serialize
swapped_bytes = serialize_node(fro_node)
print(f"Swapped model size: {len(swapped_bytes)} bytes")

# Parse swapped bytes to verify integrity
swapped_node, _ = parse_node(swapped_bytes, 0)
print(f"Parsed swapped node successfully!")
print(f"Root child count: {swapped_node['child_count']}")
for i, child in enumerate(swapped_node['children']):
    print(f"  [{i}]: type=0x{child['type_id']:05X}, size={child['data_size']}")
