import struct, sys
sys.path.insert(0, '.')
from core.esf_parser import ESFParser

with open('workspace/original/CHAR.ESF', 'rb') as f:
    van_data = f.read()
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    fro_data = f.read()

van_parser = ESFParser(van_data).parse()
fro_parser = ESFParser(fro_data).parse()

van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}

def parse_node(data, pos):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {'type_id': type_id, 'data_size': data_size, 'child_count': child_count, 'children': [], 'inline_data': None}
    pos += 12
    if child_count == 0:
        node['inline_data'] = data[pos:pos+data_size]
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child:
                node['children'].append(child)
    return node, pos

h = 0x2EF8E480

ve = van_map[h]
van_bytes = van_data[ve.offset:ve.offset+ve.length]
van_root, _ = parse_node(van_bytes, 0)

fe = fro_map[h]
fro_bytes = fro_data[fe.offset:fe.offset+fe.length]
fro_root, _ = parse_node(fro_bytes, 0)

# Compare skeleton bone data
van_bone = next((c for c in van_root['children'] if c['type_id'] == 0x0B070), None)
fro_bone = next((c for c in fro_root['children'] if c['type_id'] == 0x0B070), None)

print("=== SKELETON BONE-BY-BONE COMPARISON ===")
print(f"Vanilla bone groups: {van_bone['child_count']}")
print(f"Frontiers bone groups: {fro_bone['child_count']}")

for i in range(min(van_bone['child_count'], fro_bone['child_count'])):
    vb = van_bone['children'][i]
    fb = fro_bone['children'][i]
    # Each 0x0B000 has sub-children: 0x0B010 (header) and 0x0B020 (weight data)
    v_header = next((c for c in vb['children'] if c['type_id'] == 0x0B010), None)
    f_header = next((c for c in fb['children'] if c['type_id'] == 0x0B010), None)
    v_weights = next((c for c in vb['children'] if c['type_id'] == 0x0B020), None)
    f_weights = next((c for c in fb['children'] if c['type_id'] == 0x0B020), None)
    
    match = "MATCH" if (v_header and f_header and v_header['inline_data'] == f_header['inline_data']) else "DIFFER"
    w_match = "MATCH" if (v_weights and f_weights and v_weights['data_size'] == f_weights['data_size']) else "DIFFER"
    
    print(f"  Bone {i}: header={match}, weights_size: V={v_weights['data_size'] if v_weights else 0} F={f_weights['data_size'] if f_weights else 0} ({w_match})")

# Compare the mesh leaf node sizes more carefully
van_mesh = next((c for c in van_root['children'] if c['type_id'] == 0x02610), None)
fro_mesh = next((c for c in fro_root['children'] if c['type_id'] == 0x02610), None)

print(f"\n=== MESH LEAF SIZE COMPARISON (first 20 pairs) ===")
print(f"Vanilla total mesh size: {van_mesh['data_size']}")
print(f"Frontiers total mesh size: {fro_mesh['data_size']}")
print(f"Size difference: {van_mesh['data_size'] - fro_mesh['data_size']} bytes")

for i in range(min(20, van_mesh['child_count'], fro_mesh['child_count'])):
    vc = van_mesh['children'][i]
    fc = fro_mesh['children'][i]
    delta = vc['data_size'] - fc['data_size']
    print(f"  [{i:2d}] Vanilla={vc['data_size']:6d}  Frontiers={fc['data_size']:6d}  Delta={delta:+6d}")

# Check the 0x02800 node (animation/rigging container)
van_anim = next((c for c in van_root['children'] if c['type_id'] == 0x02800), None)
fro_anim = next((c for c in fro_root['children'] if c['type_id'] == 0x02800), None)

print(f"\n=== ANIMATION/RIGGING (0x02800) COMPARISON ===")
if van_anim:
    print(f"Vanilla 0x02800: children={van_anim['child_count']}, size={van_anim['data_size']}")
else:
    print("Vanilla: NO 0x02800 node found!")
if fro_anim:
    print(f"Frontiers 0x02800: children={fro_anim['child_count']}, size={fro_anim['data_size']}")
else:
    print("Frontiers: NO 0x02800 node found!")

# Compare type lists
van_types = [hex(c['type_id']) for c in van_root['children']]
fro_types = [hex(c['type_id']) for c in fro_root['children']]
print(f"\n=== ROOT CHILD TYPE LISTS ===")
print(f"Vanilla ({len(van_types)}):  {van_types}")
print(f"Frontiers ({len(fro_types)}): {fro_types}")
