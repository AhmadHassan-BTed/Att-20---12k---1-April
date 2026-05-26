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

def find_nodes(node, type_id, lst):
    if node['type_id'] == type_id:
        lst.append(node)
    for child in node['children']:
        find_nodes(child, type_id, lst)

def inspect_links():
    with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
        van_data = f.read()
    van_node, _ = parse_node(van_data, 0)
    
    with open('workspace/frontiers_reference.bin', 'rb') as f:
        fro_data = f.read()
    fro_node, _ = parse_node(fro_data, 0)
    
    # Textures
    van_texs = van_node['children'][1]['children'][1]['children']
    fro_texs = fro_node['children'][1]['children'][1]['children']
    
    van_tex_map = {struct.unpack('<I', t['inline_data'][:4])[0]: i for i, t in enumerate(van_texs)}
    fro_tex_map = {struct.unpack('<I', t['inline_data'][:4])[0]: i for i, t in enumerate(fro_texs)}
    
    # Materials
    van_mats = van_node['children'][1]['children'][2]['children']
    fro_mats = fro_node['children'][1]['children'][2]['children']
    
    # Rendering state nodes 0x31100
    van_31100 = []
    fro_31100 = []
    find_nodes(van_node, 0x31100, van_31100)
    find_nodes(fro_node, 0x31100, fro_31100)
    
    print("=== VANILLA MATERIALS TO TEXTURES LINKING ===")
    for i, m in enumerate(van_31100):
        t_hash = struct.unpack('<I', m['inline_data'][16:20])[0]
        tex_idx = van_tex_map.get(t_hash, -1)
        t_size = len(van_texs[tex_idx]['inline_data']) if tex_idx != -1 else 0
        print(f"  Material {i}: links to Texture Hash 0x{t_hash:08X} (Texture Slot {tex_idx}, Size {t_size} B)")
        
    print("\n=== FRONTIERS MATERIALS TO TEXTURES LINKING ===")
    for i, m in enumerate(fro_31100):
        t_hash = struct.unpack('<I', m['inline_data'][16:20])[0]
        tex_idx = fro_tex_map.get(t_hash, -1)
        t_size = len(fro_texs[tex_idx]['inline_data']) if tex_idx != -1 else 0
        print(f"  Material {i}: links to Texture Hash 0x{t_hash:08X} (Texture Slot {tex_idx}, Size {t_size} B)")

if __name__ == '__main__':
    inspect_links()
