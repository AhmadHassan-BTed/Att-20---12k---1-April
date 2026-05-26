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

# Helper to find all 0x01000 nodes
def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

van_tex = []
fro_tex = []
find_nodes(van_node, 0x01000, van_tex)
find_nodes(fro_node, 0x01000, fro_tex)

print(f"Vanilla has {len(van_tex)} texture nodes.")
print(f"Frontiers has {len(fro_tex)} texture nodes.")

def dump_texture_header(tex_node, label):
    print(f"\n=== {label} (size={len(tex_node['inline_data'])} B) ===")
    data = tex_node['inline_data']
    # Print the first 64 bytes in hex
    print("Hex:   ", data[:64].hex())
    # Unpack first few DWORDS
    dwords = []
    for i in range(0, min(len(data), 32), 4):
        dwords.append(struct.unpack_from('<I', data, i)[0])
    print("Dwords:", [f"0x{dw:08X}" for dw in dwords])

dump_texture_header(van_tex[0], "Vanilla Texture 0")
dump_texture_header(fro_tex[0], "Frontiers Texture 0")
