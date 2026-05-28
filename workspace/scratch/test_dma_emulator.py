import os
import sys
import struct
import math

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser

def get_unpack_size(cmd, num):
    base_cmd = cmd & 0x0F
    fmt = (base_cmd >> 2) & 3
    sz = base_cmd & 3
    components = [1, 2, 3, 4][fmt]
    bits = [32, 16, 8, 5][sz]
    total_bits = components * bits * num
    total_bytes = (total_bits + 7) // 8
    aligned_bytes = (total_bytes + 3) & ~3
    return aligned_bytes

def parse_vif_stream(data, start, length):
    pos = start
    end = start + length
    arrays = {}
    
    while pos < end:
        vif_code = struct.unpack_from('<I', data, pos)[0]
        imm = vif_code & 0xFFFF
        num = (vif_code >> 16) & 0xFF
        cmd = (vif_code >> 24) & 0xFF
        
        pos += 4
        
        if 0x60 <= cmd <= 0x7F:
            if num == 0: num = 256
            data_size = get_unpack_size(cmd, num)
            base_vif_cmd = cmd & ~0x10
            arrays[base_vif_cmd] = arrays.get(base_vif_cmd, [])
            arrays[base_vif_cmd].append({
                'vif_offset': pos - 4,
                'data_offset': pos,
                'size': data_size,
                'num': num,
                'raw_cmd': cmd
            })
            pos += data_size
            pos = (pos + 3) & ~3
        elif cmd == 0x50 or cmd == 0x51:
            pos += imm * 16
    return arrays

def print_file_stats(filename, target_hash):
    with open(filename, 'rb') as f:
        data = f.read()
    esf = ESFParser(data).parse()
    
    for entry in esf.pointer_table:
        if entry.asset_id == target_hash:
            def search_tree(node, target_offset):
                if node['offset'] == target_offset: return node
                for c in node.get('children', []):
                    res = search_tree(c, target_offset)
                    if res: return res
                return None
            
            model = search_tree(esf.root, entry.offset)
            def find_geom(node):
                if node['type_id'] == 0x02610: return node
                for c in node.get('children', []):
                    res = find_geom(c)
                    if res: return res
                return None
            
            geom = find_geom(model)
            if not geom: return
            
            geom_data = data[geom['offset']+12 : geom['offset']+12+geom['data_size']]
            print(f"[{filename}] Geometry DMA Size: {len(geom_data)}")
            
            pos = 15 & ~15
            
            while pos + 16 <= len(geom_data):
                dma_tag = struct.unpack_from('<Q', geom_data, pos)[0]
                qwc = dma_tag & 0xFFFF
                id_val = (dma_tag >> 28) & 0x7
                id_names = {0:'refe', 1:'cnt', 2:'next', 3:'ref', 4:'refs', 5:'call', 6:'ret', 7:'end'}
                id_name = id_names.get(id_val, 'unknown')
                
                packet_data_start = pos + 16
                packet_data_len = qwc * 16
                
                if qwc > 0:
                    arrays = parse_vif_stream(geom_data, packet_data_start, packet_data_len)
                    if arrays:
                        print(f"  Packet {id_name}: QWC {qwc}")
                        for cmd in [0x6C, 0x6D, 0x6A]:
                            if cmd in arrays:
                                nums = [x['num'] for x in arrays[cmd]]
                                print(f"    CMD 0x{cmd:02X}: {len(arrays[cmd])} arrays. Nums: {nums}")
                
                if id_name == 'end':
                    break
                    
                pos += 16 + packet_data_len

def debug():
    print_file_stats("workspace/original/CHAR.ESF", 0x05AEBA67)
    print_file_stats("workspace/FINAL_CHAR_MERGED.ESF", 0x05AEBA67)

if __name__ == "__main__":
    debug()
