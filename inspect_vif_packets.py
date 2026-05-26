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

def dump_vif_packets_detailed(data, start_offset, count=32):
    print(f"Dumping VIF starting at offset 0x{start_offset:X}:")
    pos = start_offset
    for _ in range(count):
        if pos >= len(data):
            break
        val = struct.unpack_from('<I', data, pos)[0]
        cmd = val >> 24
        
        # Check command
        if 0x60 <= cmd <= 0x7F:
            # UNPACK VIFcode
            # bits:
            # 31-24: 01xxxxxx (UNPACK)
            # 27-26: VL (vector length: 0=1, 1=2, 2=3, 3=4)
            # 25-24: VN (component size: 0=32bit, 1=16bit, 2=8bit, 3=reserved)
            # 23-16: NUM (amount of vectors to write)
            # 15: M (masking flag)
            # 14: US (unsigned/signed flag: 0=signed, 1=unsigned)
            # 13-10: zero
            # 9-0: ADDR (destination VU1 memory address)
            vl = (val >> 26) & 3
            vn = (val >> 24) & 3
            num = (val >> 16) & 0xFF
            m = (val >> 15) & 1
            us = (val >> 14) & 1
            addr = val & 0x3FF
            
            # Format name
            vlen = vl + 1 # 1-4 components
            bits = 32 if vn == 0 else (16 if vn == 1 else 8)
            fmt_str = f"V{vlen}-{bits}bit"
            if us:
                fmt_str = "U" + fmt_str
            else:
                fmt_str = "S" + fmt_str
                
            print(f"  [{pos:04X}] UNPACK: {fmt_str}, num={num}, mask={m}, unsigned={us}, addr=0x{addr:03X} | Raw: 0x{val:08X}")
            pos += 4
            
            # We don't advance pos by the unpacked data size because in ESF nodes,
            # are the raw data vectors interleaved or does the VIF packet represent
            # a serialized VIF stream sent to VU1?
            # Wait! In PS2 hardware, the VIF codes are followed by their immediate data!
            # Let's check if the immediate data follows the VIFcode in the binary node!
            # The immediate data size (in bytes) is:
            # (component_size_in_bytes) * (vector_length) * num, rounded up to 4 bytes!
            comp_bytes = 4 if vn == 0 else (2 if vn == 1 else 1)
            data_bytes = comp_bytes * vlen * num
            if data_bytes % 4 != 0:
                data_bytes += 4 - (data_bytes % 4)
                
            # Print a few bytes of the data
            if data_bytes > 0:
                data_hex = data[pos:pos+min(data_bytes, 16)].hex()
                print(f"         Data ({data_bytes} bytes): {data_hex} ...")
                pos += data_bytes
        elif cmd == 0x30:
            cl = (val >> 8) & 0xFF
            wl = val & 0xFF
            print(f"  [{pos:04X}] STCYCL: cl={cl}, wl={wl} | Raw: 0x{val:08X}")
            pos += 4
        elif cmd == 0x10:
            print(f"  [{pos:04X}] STMASK: Raw: 0x{val:08X}")
            pos += 8
        elif cmd == 0x20:
            print(f"  [{pos:04X}] STROW: Raw: 0x{val:08X}")
            pos += 16
        elif cmd == 0x00:
            print(f"  [{pos:04X}] NOP | Raw: 0x{val:08X}")
            pos += 4
        else:
            print(f"  [{pos:04X}] Unknown / Immediate Data: Raw: 0x{val:08X}")
            pos += 4

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

print("=== VANILLA VIF DECODING ===")
dump_vif_packets_detailed(van_geometry_nodes[0]['inline_data'], 0x60, 15)

print("\n=== FRONTIERS VIF DECODING ===")
dump_vif_packets_detailed(fro_geometry_nodes[0]['inline_data'], 0x60, 15)
