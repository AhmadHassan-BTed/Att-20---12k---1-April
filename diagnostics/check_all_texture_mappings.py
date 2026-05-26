import json
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

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    with open(original_esf, 'rb') as f:
        van_data = f.read()
    van_parser = ESFParser(van_data).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    with open(expansion_esf, 'rb') as f:
        fro_data = f.read()
    fro_parser = ESFParser(fro_data).parse()
    fro_map = {e.asset_id: e for e in fro_parser.pointer_table if e.asset_id is not None}
    
    print("=== DUMPING TEXTURE SIZE STRUCTURES FOR ALL 11 MODELS ===")
    for idx, t in enumerate(targets):
        h = int(t['original_hash'], 16)
        print(f"\n[{idx+1}/11] Model 0x{h:08X}:")
        
        van_entry = van_map[h]
        van_model_bytes = van_data[van_entry.offset : van_entry.offset + van_entry.length]
        van_root, _ = parse_node(van_model_bytes, 0)
        van_texs = van_root['children'][1]['children'][1]['children']
        
        fro_entry = fro_map[h]
        fro_model_bytes = fro_data[fro_entry.offset : fro_entry.offset + fro_entry.length]
        fro_root, _ = parse_node(fro_model_bytes, 0)
        fro_texs = fro_root['children'][1]['children'][1]['children']
        
        van_sizes = []
        for t_node in van_texs:
            w = struct.unpack_from('<I', t_node['inline_data'], 4)[0]
            h_val = struct.unpack_from('<I', t_node['inline_data'], 8)[0]
            van_sizes.append(f"{w}x{h_val}")
            
        fro_sizes = []
        for t_node in fro_texs:
            w = struct.unpack_from('<I', t_node['inline_data'], 4)[0]
            h_val = struct.unpack_from('<I', t_node['inline_data'], 8)[0]
            fro_sizes.append(f"{w}x{h_val}")
            
        print("  Vanilla texture sizes:  ", van_sizes)
        print("  Frontiers texture sizes:", fro_sizes)
        
        # Check if they match directly
        if van_sizes == fro_sizes:
            print("  [MATCH] Direct sequential mapping matches perfectly!")
        else:
            print("  [MISMATCH] Size structures differ! Mapping is required.")

if __name__ == '__main__':
    main()
