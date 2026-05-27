import json
import struct
import sys
import os
sys.path.append(os.getcwd())
from core.esf_parser import ESFParser

def parse_node(data, pos, depth=0):
    if pos + 12 > len(data): return None, pos
    tid = struct.unpack_from('<I', data, pos)[0]
    sz  = struct.unpack_from('<I', data, pos + 4)[0]
    cnt = struct.unpack_from('<I', data, pos + 8)[0]
    print(f"{'  '*depth}[Type: 0x{tid:05X}] Size: {sz}, Children: {cnt}")
    pos += 12
    if cnt == 0:
        pos += sz
    else:
        for _ in range(cnt):
            child, pos = parse_node(data, pos, depth+1)
    return True, pos

with open("workspace/original/CHAR.ESF", "rb") as f:
    van_data = f.read()

parser = ESFParser(van_data).parse()
pt_map = {e.asset_id: e for e in parser.pointer_table if e.asset_id is not None}
hash_val = 0x05AEBA67
entry = pt_map[hash_val]

print("--- Vanilla Payload Tree ---")
parse_node(van_data[entry.offset:entry.offset+entry.length], 0)
