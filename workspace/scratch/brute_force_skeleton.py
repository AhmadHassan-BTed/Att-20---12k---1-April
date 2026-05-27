import sys
import struct
import os

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

def dump_hex(data, limit=128):
    sz = min(len(data), limit)
    return " ".join([f"{b:02X}" for b in data[:sz]])

def diagnostic_search(root_node, expected_counts, title):
    print(f"\n--- {title} ---")
    
    exact_sizes = []
    for cnt in expected_counts:
        exact_sizes.extend([cnt * 64, cnt * 80])
        
    def search(node):
        if node['cnt'] == 0 and node['data']:
            sz = len(node['data'])
            
            # Exact Match
            if sz in exact_sizes:
                print(f"[!] EXACT MATCH! Node 0x{node['type']:05X} (Size: {sz} bytes)")
                print(f"    Hex: {dump_hex(node['data'])}")
                return
                    
        for child in node['children']:
            search(child)
            
    search(root_node)

def main():
    # Load Frontiers Payload
    fro_path = 'workspace/payloads/asset_0x05AEBA67.bin'
    if os.path.exists(fro_path):
        with open(fro_path, 'rb') as f:
            fro_root, _ = parse_node(f.read(), 0)
        diagnostic_search(fro_root, [43], "FRONTIERS SEARCH (Target: 43 bones)")
    
    # We can load the same original/CHAR.ESF logic to find the vanilla payload
    van_path = 'workspace/scratch/temp_vanilla_source.bin'
    # Actually, let's just parse original/CHAR.ESF and grab the first payload
    from core.esf_parser import ESFParser
    with open('workspace/original/CHAR.ESF', 'rb') as f:
        van_data = f.read()
    parser = ESFParser(van_data).parse()
    pt_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}
    if 0x05AEBA67 in pt_map:
        entry = pt_map[0x05AEBA67]
        van_root, _ = parse_node(van_data[entry.offset:entry.offset+entry.length], 0)
        diagnostic_search(van_root, [32], "VANILLA SEARCH (Target: 32 bones)")

if __name__ == "__main__":
    sys.path.append(os.getcwd())
    main()
