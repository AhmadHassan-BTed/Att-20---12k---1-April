import struct
import sys

def parse_node(data, pos):
    if pos + 12 > len(data): return None, pos
    tid = struct.unpack_from('<I', data, pos)[0]
    sz  = struct.unpack_from('<I', data, pos + 4)[0]
    cnt = struct.unpack_from('<I', data, pos + 8)[0]
    node = {'type': tid, 'size': sz, 'cnt': cnt, 'children': [], 'data': None, 'offset': pos}
    pos += 12
    if cnt == 0:
        node['data'] = data[pos:pos+sz]
        pos += sz
    else:
        for _ in range(cnt):
            child, pos = parse_node(data, pos)
            if child: node['children'].append(child)
    return node, pos

def search(node):
    if node['cnt'] == 0 and node['data']:
        sz = len(node['data'])
        if sz % 43 == 0 and sz > 0:
            print(f"[Frontiers] Found node 0x{node['type']:05X} (Size: {sz}), Element Size: {sz//43} bytes")
        if sz % 32 == 0 and sz > 0:
            print(f"[Vanilla] Found node 0x{node['type']:05X} (Size: {sz}), Element Size: {sz//32} bytes")
            
    if node['cnt'] == 43:
        print(f"[Frontiers] Found node 0x{node['type']:05X} with exactly 43 children!")
    if node['cnt'] == 32:
        print(f"[Vanilla] Found node 0x{node['type']:05X} with exactly 32 children!")
        
    for child in node['children']:
        search(child)

print("--- Frontiers Payload ---")
with open('workspace/payloads/asset_0x05AEBA67.bin', 'rb') as f:
    fro_root, _ = parse_node(f.read(), 0)
    search(fro_root)
