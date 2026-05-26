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

van_02610 = [c for c in van_node['children'] if c['type_id'] == 0x02610][0]
fro_02610 = [c for c in fro_node['children'] if c['type_id'] == 0x02610][0]

def check_strip_vif(strip_node, label):
    data = strip_node['inline_data']
    print(f"\n--- Checking VIF inside {label} (size={len(data)} B) ---")
    
    # Scan for any UNPACK VIFcodes (0x60000000 - 0x7FFFFFFF in little-endian)
    vif_unpacks = 0
    vif_mscals = 0
    vif_stcycls = 0
    
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack_from('<I', data, i)[0]
        cmd = val >> 24
        if 0x60 <= cmd <= 0x7F:
            vif_unpacks += 1
            if vif_unpacks <= 3:
                print(f"  [{i:04X}] UNPACK: Raw=0x{val:08X}")
        elif cmd == 0x14:
            vif_mscals += 1
            print(f"  [{i:04X}] MSCAL: addr=0x{val & 0xFFFF:03X} | Raw=0x{val:08X}")
        elif cmd == 0x30:
            vif_stcycls += 1
            if vif_stcycls <= 3:
                print(f"  [{i:04X}] STCYCL: Raw=0x{val:08X}")
                
    print(f"Summary: UNPACK={vif_unpacks}, MSCAL={vif_mscals}, STCYCL={vif_stcycls}")

check_strip_vif(van_02610['children'][0], "Vanilla Strip 0")
check_strip_vif(fro_02610['children'][0], "Frontiers Strip 0")
