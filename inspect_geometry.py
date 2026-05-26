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

def dump_geometry_header(data):
    print("=== DUMPING GEOMETRY HEADER IN MULTIPLE FORMATS ===")
    print("Offset | Hex      | float     | uint32     | uint16 (x2)   | int16 (x2)")
    print("-" * 80)
    for i in range(0, min(len(data), 160), 4):
        raw = data[i:i+4]
        val_hex = raw.hex()
        val_float = struct.unpack_from('<f', data, i)[0]
        val_uint32 = struct.unpack_from('<I', data, i)[0]
        val_uint16 = struct.unpack_from('<HH', data, i)
        val_int16 = struct.unpack_from('<hh', data, i)
        print(f"0x{i:03X}  | {val_hex} | {val_float:>9.4f} | {val_uint32:>10d} | {str(val_uint16):<13} | {str(val_int16):<13}")

with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

van_geometry_nodes = []
fro_geometry_nodes = []
find_nodes(van_node, 0x21210, van_geometry_nodes)
find_nodes(fro_node, 0x21210, fro_geometry_nodes)

print("\n--- VANILLA GEOMETRY HEADER ---")
dump_geometry_header(van_geometry_nodes[0]['inline_data'])

print("\n--- FRONTIERS GEOMETRY HEADER ---")
dump_geometry_header(fro_geometry_nodes[0]['inline_data'])
