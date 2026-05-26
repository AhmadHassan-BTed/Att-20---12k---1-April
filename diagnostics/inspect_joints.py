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

# Helper to find 0x02A50 (bone joint data)
def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

van_joints = []
fro_joints = []
find_nodes(van_node, 0x02A50, van_joints)
find_nodes(fro_node, 0x02A50, fro_joints)

print(f"Vanilla has {len(van_joints)} joint nodes.")
print(f"Frontiers has {len(fro_joints)} joint nodes.")

def dump_joint(joint_node, label):
    print(f"\n=== {label} ===")
    data = joint_node['inline_data']
    print("Hex:   ", data.hex())
    # Unpack as floats
    floats = struct.unpack('<7f', data)
    print("Floats:", [f"{f:.4f}" for f in floats])

dump_joint(van_joints[0], "Vanilla Joint 0")
dump_joint(fro_joints[0], "Frontiers Joint 0")
