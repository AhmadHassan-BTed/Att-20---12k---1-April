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

def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

van_geom = []
fro_geom = []
find_nodes(van_node, 0x21210, van_geom)
find_nodes(fro_node, 0x21210, fro_geom)

def dump_raw_hex(data, limit=256):
    for i in range(0, min(len(data), limit), 16):
        chunk = data[i:i+16]
        hex_str = ' '.join(f"{b:02X}" for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        print(f"  0x{i:03X}: {hex_str:<47}  {ascii_str}")

print("=== VANILLA 0x21210 RAW HEX ===")
dump_raw_hex(van_geom[0]['inline_data'], 256)

print("\n=== FRONTIERS 0x21210 RAW HEX ===")
dump_raw_hex(fro_geom[0]['inline_data'], 256)
