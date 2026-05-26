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

print("[*] Parsing Frontiers reference...")
with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

fro_02610 = [c for c in fro_node['children'] if c['type_id'] == 0x02610][0]

total_mscals = 0
for idx, strip in enumerate(fro_02610['children']):
    data = strip['inline_data']
    mscals = 0
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack_from('<I', data, i)[0]
        cmd = val >> 24
        if cmd == 0x14:
            mscals += 1
    if mscals > 0:
        print(f"  Strip {idx}: {mscals} MSCAL codes found")
    total_mscals += mscals

print(f"[+] Total MSCAL codes in all {len(fro_02610['children'])} strips of Frontiers 0x2EF8E480: {total_mscals}")
