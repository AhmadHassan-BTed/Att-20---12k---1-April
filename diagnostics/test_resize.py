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

# Test round-trip with update_node_sizes
with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    orig = f.read()

node, _ = parse_node(orig, 0)
update_node_sizes(node)
serialized = serialize_node(node)

if orig == serialized:
    print("[PASS] Sizes match perfectly, round-trip matches!")
else:
    print("[FAIL] Mismatch!")
