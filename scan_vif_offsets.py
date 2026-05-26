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

van_geometry_nodes = []
fro_geometry_nodes = []
find_nodes(van_node, 0x21210, van_geometry_nodes)
find_nodes(fro_node, 0x21210, fro_geometry_nodes)

def find_vif_start(data):
    # Scan for the first real VIFcode STCYCL (0x30) or STMASK (0x10) or UNPACK (0x60 - 0x7F)
    # that is part of a valid VIF packet.
    # Usually, VIF packets are 8-byte or 16-byte aligned.
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack_from('<I', data, i)[0]
        cmd = val >> 24
        if 0x60 <= cmd <= 0x7F:
            # Check if this UNPACK looks valid (num > 0, addr < 0x400)
            num = (val >> 16) & 0xFF
            addr = val & 0x3FF
            if num > 0 and addr < 0x400:
                return i
    return -1

for idx, vn in enumerate(van_geometry_nodes):
    vstart = find_vif_start(vn['inline_data'])
    print(f"Vanilla Geometry Node {idx}: size={len(vn['inline_data'])} B, first valid UNPACK at offset 0x{vstart:X}")
    if vstart != -1:
        print(f"  Unpack VIFcode: 0x{struct.unpack_from('<I', vn['inline_data'], vstart)[0]:08X}")

for idx, fn in enumerate(fro_geometry_nodes):
    vstart = find_vif_start(fn['inline_data'])
    print(f"Frontiers Geometry Node {idx}: size={len(fn['inline_data'])} B, first valid UNPACK at offset 0x{vstart:X}")
    if vstart != -1:
        print(f"  Unpack VIFcode: 0x{struct.unpack_from('<I', fn['inline_data'], vstart)[0]:08X}")
