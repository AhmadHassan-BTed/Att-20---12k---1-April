import struct
import sys

def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
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

def print_tree(node, depth=0):
    print(f"{'  '*depth}[Type: 0x{node['type_id']:05X}] Size: {node['data_size']}, Children: {node['child_count']}")
    for c in node['children']:
        print_tree(c, depth+1)

with open('workspace/payloads/asset_0x05AEBA67.bin', 'rb') as f:
    fro_data = f.read()

print("--- Payload Tree ---")
fro_root, _ = parse_node(fro_data, 0)
print_tree(fro_root)
