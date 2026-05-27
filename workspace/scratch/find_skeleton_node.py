import sys
import struct
import json

def parse_node(data, pos):
    if pos + 12 > len(data): return None, pos
    tid = struct.unpack_from('<I', data, pos)[0]
    sz  = struct.unpack_from('<I', data, pos + 4)[0]
    cnt = struct.unpack_from('<I', data, pos + 8)[0]
    node = {'type': tid, 'size': sz, 'cnt': cnt, 'children': [], 'data': None}
    pos += 12
    if cnt == 0:
        node['data'] = data[pos:pos+sz]
        pos += sz
    else:
        for _ in range(cnt):
            child, pos = parse_node(data, pos)
            if child: node['children'].append(child)
    return node, pos

def search_skeleton(node, target_count):
    if node['cnt'] == 0 and node['data']:
        # Check if size is an exact multiple of 64 or 80 or similar
        sz = len(node['data'])
        if sz == target_count * 64:
            return node['type'], 64
        elif sz == target_count * 80:
            return node['type'], 80
        elif sz == target_count * 48:
            return node['type'], 48
            
    for child in node['children']:
        res = search_skeleton(child, target_count)
        if res: return res
    return None

def analyze(filepath, target_count):
    with open(filepath, 'rb') as f:
        data = f.read()
    # Find the character container (0x05AEBA67)
    # The file is CHAR.ESF so we need to find 0x72700 / 0x62700 with the asset hash.
    # Actually just parse the first node if it's a payload.
    # We will just parse the payload!
    root, _ = parse_node(data, 0)
    res = search_skeleton(root, target_count)
    if res:
        print(f"Found Skeleton for {target_count} bones in Type: 0x{res[0]:05X}, Element Size: {res[1]}")
    else:
        print(f"No skeleton found for {target_count} bones.")
        
analyze('workspace/payloads/asset_0x05AEBA67.bin', 43)
# To get vanilla, we need to extract a vanilla payload, or just use the target_assets.json
