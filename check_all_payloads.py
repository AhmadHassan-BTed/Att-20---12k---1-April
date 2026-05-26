import glob, os, struct

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
            child, next_pos = parse_node(data, next_pos)
            node['children'].append(child)
            
    return node, next_pos

bin_files = sorted(glob.glob('workspace/payloads/*.bin'))
for filepath in bin_files:
    filename = os.path.basename(filepath)
    with open(filepath, 'rb') as f:
        data = f.read()
    
    # We want to make sure it's a character model payload (type_id 0x62700 or patched 0x72700)
    type_id = struct.unpack_from('<I', data, 0)[0]
    if type_id not in (0x62700, 0x72700):
        # Skip dependency payloads (e.g. textures, sub-meshes) which are also in payloads directory
        continue
        
    node, _ = parse_node(data, 0)
    child_types = [f"0x{c['type_id']:05X}" for c in node['children']]
    print(f"{filename}: type=0x{node['type_id']:05X}, children={node['child_count']}, types={child_types}")
