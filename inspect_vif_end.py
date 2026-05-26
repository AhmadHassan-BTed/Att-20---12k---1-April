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

def dump_vif_end(data, label):
    print(f"\n=== {label} (last 128 bytes) ===")
    end_data = data[-128:]
    pos = 0
    while pos < len(end_data):
        val = struct.unpack_from('<I', end_data, pos)[0]
        cmd = val >> 24
        
        # Check standard VIFcodes
        if cmd == 0x00:
            print(f"  [{pos:02X}] NOP | Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x10:
            print(f"  [{pos:02X}] STMASK: Raw: 0x{val:08X}")
            pos += 8
        elif cmd == 0x20:
            print(f"  [{pos:02X}] STROW: Raw: 0x{val:08X}")
            pos += 16
        elif cmd == 0x30:
            print(f"  [{pos:02X}] STCYCL: Raw: 0x{val:08X}")
            pos += 4
        elif 0x60 <= cmd <= 0x7F:
            print(f"  [{pos:02X}] UNPACK: Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x14:
            # MSCAL
            addr = val & 0xFFFF
            print(f"  [{pos:02X}] MSCAL: addr=0x{addr:03X} | Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x15:
            # MSCNT
            print(f"  [{pos:02X}] MSCNT | Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x17:
            # FLUSH
            print(f"  [{pos:02X}] FLUSH | Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x13:
            # DIRECT
            print(f"  [{pos:02X}] DIRECT | Raw: 0x{val:08X}")
            pos += 4
        else:
            print(f"  [{pos:02X}] Data: Raw: 0x{val:08X}")
            pos += 4

dump_vif_end(van_geom[0]['inline_data'], "Vanilla Geometry Node 0")
dump_vif_end(fro_geom[0]['inline_data'], "Frontiers Geometry Node 0")
