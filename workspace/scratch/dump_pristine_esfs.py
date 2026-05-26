import sys
import struct
sys.path.append(r't:\Att 20 - 12k - 1 April')
from core.esf_parser import ESFParser

def parse_node(data, pos):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
        'offset': pos
    }
    pos += 12
    if child_count == 0:
        node['inline_data'] = data[pos : pos + data_size]
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def dump_tree(node, depth=0):
    indent = "  " * depth
    print(f"{indent}- Type 0x{node['type_id']:05X} (size={node['data_size']:,}, children={node['child_count']})")
    for child in node['children']:
        dump_tree(child, depth + 1)

def main():
    original_esf = 'workspace/original/CHAR.ESF'
    expansion_esf = 'workspace/expansion/CHAR.ESF'
    target_hash = 0x2EF8E480 # Human Male
    
    print("Parsing original...")
    with open(original_esf, 'rb') as f:
        van_data = f.read()
    van_parser = ESFParser(van_data).parse()
    van_entry = next(e for e in van_parser.pointer_table if e.asset_id == target_hash)
    van_bytes = van_data[van_entry.offset : van_entry.offset + van_entry.length]
    van_root, _ = parse_node(van_bytes, 0)
    
    print("\n==========================================")
    print("  VANILLA MODEL TREE (ON DISK)")
    print("==========================================")
    dump_tree(van_root)
    
    print("\nParsing expansion...")
    with open(expansion_esf, 'rb') as f:
        fro_data = f.read()
    fro_parser = ESFParser(fro_data).parse()
    fro_entry = next(e for e in fro_parser.pointer_table if e.asset_id == target_hash)
    fro_bytes = fro_data[fro_entry.offset : fro_entry.offset + fro_entry.length]
    fro_root, _ = parse_node(fro_bytes, 0)
    
    print("\n==========================================")
    print("  FRONTIERS MODEL TREE (ON DISK)")
    print("==========================================")
    dump_tree(fro_root)

if __name__ == '__main__':
    main()
