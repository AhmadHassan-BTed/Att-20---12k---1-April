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

print("[*] Parsing Vanilla and Frontiers models...")
with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

van_skeleton = [c for c in van_node['children'] if c['type_id'] == 0x12400][0]
fro_skeleton = [c for c in fro_node['children'] if c['type_id'] == 0x22400][0]

print(f"Vanilla skeleton 0x12400 child_count: {van_skeleton['child_count']}, size: {van_skeleton['data_size']}")
print(f"Frontiers skeleton 0x22400 child_count: {fro_skeleton['child_count']}, size: {fro_skeleton['data_size']}")

def dump_skeleton_data(skel_node, label):
    print(f"\n=== {label} Detailed Dump (first 256 bytes) ===")
    data = skel_node['inline_data']
    print(f"Total Bytes: {len(data)}")
    print("Hex: ", data[:128].hex())
    # Unpack as floats and integers to see the pattern
    print("As Int32:")
    for i in range(0, min(len(data), 64), 4):
        val = struct.unpack_from('<i', data, i)[0]
        fval = struct.unpack_from('<f', data, i)[0]
        print(f"  [{i:03X}]: Int={val:11d} | Float={fval:>12.5f}")

dump_skeleton_data(van_skeleton, "Vanilla 0x12400")
dump_skeleton_data(fro_skeleton, "Frontiers 0x22400")
