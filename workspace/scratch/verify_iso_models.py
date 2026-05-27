import os
import sys
import struct
import json

sys.path.append(r't:\Att 20 - 12k - 1 April')
from core.esf_parser import ESFParser

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

def main():
    iso_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
    json_path = 'workspace/target_assets.json'
    
    if not os.path.exists(iso_path):
        print(f"[-] Error: Patched ISO not found at {iso_path}")
        return
        
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    # Read LBA 1492368 from ISO
    # Each sector is 2048 bytes
    lba = 1492368
    offset = lba * 2048
    print(f"[*] Reading CHAR.ESF from ISO at offset 0x{offset:X} (LBA {lba})...")
    
    with open(iso_path, 'rb') as f:
        f.seek(offset)
        # We need to read the ESF file. What is the size?
        # Recompiled ESF size is around 148,810,558 bytes
        esf_size = 148810558
        esf_bytes = f.read(esf_size)
        
    print(f"[+] Read {len(esf_bytes):,} bytes from ISO sector.")
    
    # Parse ESF
    print("[*] Parsing ESF bytes from ISO...")
    parser = ESFParser(esf_bytes).parse()
    pt_map = {e.asset_id: e for e in parser.pointer_table}
    
    print("\n=== Verifying Models Written in the Patched ISO ===")
    for t in targets:
        h = int(t['expansion_hash'], 16)
        if h in pt_map:
            entry = pt_map[h]
            print(f"Model 0x{h:08X}: offset=0x{entry.offset:X}, len={entry.length:,}, type=0x{entry.type_id:05X}")
            # Slice and check first node
            node_bytes = esf_bytes[entry.offset : entry.offset + entry.length]
            root_node, _ = parse_node(node_bytes, 0)
            print(f"  Parsed Root: type=0x{root_node['type_id']:05X}, size={root_node['data_size']:,}, child_count={root_node['child_count']}")
        else:
            print(f"Model 0x{h:08X}: NOT FOUND in ISO pointer table!")

if __name__ == '__main__':
    main()
