import struct
from esf_parser import ESFParser

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

print("[*] Parsing Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

native_62700_entry = None
for entry in exp_parser.pointer_table:
    if entry.asset_id == 0x4BD83120:
        native_62700_entry = entry
        break

native_model_bytes = exp_data[native_62700_entry.offset : native_62700_entry.offset + native_62700_entry.length]
native_node, _ = parse_node(native_model_bytes, 0)

native_02610 = [c for c in native_node['children'] if c['type_id'] == 0x02610][0]

def check_strip_vif(strip_node, label):
    data = strip_node['inline_data']
    print(f"\n--- Checking VIF inside {label} (size={len(data)} B) ---")
    
    vif_unpacks = 0
    vif_mscals = 0
    vif_stcycls = 0
    
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack_from('<I', data, i)[0]
        cmd = val >> 24
        if 0x60 <= cmd <= 0x7F:
            vif_unpacks += 1
        elif cmd == 0x14:
            vif_mscals += 1
            print(f"  [{i:04X}] MSCAL: addr=0x{val & 0xFFFF:03X} | Raw=0x{val:08X}")
        elif cmd == 0x30:
            vif_stcycls += 1
            
    print(f"Summary: UNPACK={vif_unpacks}, MSCAL={vif_mscals}, STCYCL={vif_stcycls}")

for i in range(min(3, len(native_02610['children']))):
    check_strip_vif(native_02610['children'][i], f"Native 0x62700 Strip {i}")
