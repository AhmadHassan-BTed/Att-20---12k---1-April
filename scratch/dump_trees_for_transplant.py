import sys
import json
import os
sys.path.append(os.path.abspath(os.path.dirname(__file__) + '/..'))
from core.esf_parser import ESFParser

def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    import struct
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {'type_id': type_id, 'data_size': data_size, 'child_count': child_count, 'children': []}
    pos += 12
    if child_count == 0:
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def dump_tree(node, depth=0):
    indent = "  " * depth
    print(f"{indent}Type: 0x{node['type_id']:05X}, Size: {node['data_size']}, Children: {node['child_count']}")
    for c in node['children']:
        dump_tree(c, depth+1)

with open('workspace/target_assets.json', 'r') as f:
    targets = json.load(f)

h = int(targets[0]['expansion_hash'], 16)
print(f"Target hash: 0x{h:08X}")

with open('workspace/original/CHAR.ESF', 'rb') as f:
    van_bytes = f.read()
van_parser = ESFParser(van_bytes).parse()
van_entry = next(e for e in van_parser.pointer_table if e.asset_id == h)
van_data = van_bytes[van_entry.offset : van_entry.offset + van_entry.length]
van_node, _ = parse_node(van_data, 0)
print("\n--- VANILLA TREE ---")
dump_tree(van_node)

with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    fro_bytes = f.read()
fro_parser = ESFParser(fro_bytes).parse()
fro_entry = next(e for e in fro_parser.pointer_table if e.asset_id == h)
fro_data = fro_bytes[fro_entry.offset : fro_entry.offset + fro_entry.length]
fro_node, _ = parse_node(fro_data, 0)
print("\n--- FRONTIERS TREE ---")
dump_tree(fro_node)
