import os, struct
from esf_parser import ESFParser

def parse_node(data, pos, depth=0):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node = {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'offset': pos,
        'children': [],
        'inline_data': None
    }
    
    next_pos = pos + 12
    if child_count == 0:
        node['inline_data'] = data[next_pos:next_pos+data_size]
        next_pos += data_size
    else:
        for _ in range(child_count):
            child, next_pos = parse_node(data, next_pos, depth + 1)
            node['children'].append(child)
            
    return node, next_pos

print("[*] Parsing frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

# Find the first entry in Frontiers that is type 0x62700 natively (asset 0x4BD83120)
native_62700_entry = None
for entry in exp_parser.pointer_table:
    if entry.type_id == 0x62700:
        native_62700_entry = entry
        break

native_model_bytes = exp_data[native_62700_entry.offset : native_62700_entry.offset + native_62700_entry.length]
native_node, _ = parse_node(native_model_bytes, 0)

# Helper to find all 0x21210 nodes
def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

native_geom = []
find_nodes(native_node, 0x21210, native_geom)

def dump_vif_packets_brief(data, start_offset, label, count=8):
    print(f"\n--- VIF Decoding for {label} ---")
    pos = start_offset
    for _ in range(count):
        if pos >= len(data):
            break
        val = struct.unpack_from('<I', data, pos)[0]
        cmd = val >> 24
        
        if 0x60 <= cmd <= 0x7F:
            vl = (val >> 26) & 3
            vn = (val >> 24) & 3
            num = (val >> 16) & 0xFF
            m = (val >> 15) & 1
            us = (val >> 14) & 1
            addr = val & 0x3FF
            
            vlen = vl + 1
            bits = 32 if vn == 0 else (16 if vn == 1 else 8)
            fmt_str = f"V{vlen}-{bits}bit"
            if us:
                fmt_str = "U" + fmt_str
            else:
                fmt_str = "S" + fmt_str
                
            print(f"  [{pos:04X}] UNPACK: {fmt_str}, num={num}, addr=0x{addr:03X} | Raw: 0x{val:08X}")
            pos += 4
            comp_bytes = 4 if vn == 0 else (2 if vn == 1 else 1)
            data_bytes = comp_bytes * vlen * num
            if data_bytes % 4 != 0:
                data_bytes += 4 - (data_bytes % 4)
            pos += data_bytes
        elif cmd == 0x30:
            print(f"  [{pos:04X}] STCYCL: Raw: 0x{val:08X}")
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
            print(f"  [{pos:04X}] Data: Raw: 0x{val:08X}")
            pos += 4

dump_vif_packets_brief(native_geom[0]['inline_data'], 0x60, "Native Frontiers 0x62700 Model (0x4BD83120)")
