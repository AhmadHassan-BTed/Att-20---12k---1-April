import os
import sys
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def find_model_node(parser, target_hash):
    for child in parser.root['children']:
        if child['type_id'] == 0x0A010:
            for model_node in child['children']:
                h = parser._find_hash_in_subtree(model_node)
                if h == target_hash:
                    return model_node
    return None

def find_node(node, type_id):
    if node['type_id'] == type_id:
        return node
    for child in node['children']:
        res = find_node(child, type_id)
        if res: return res
    return None

def extract_vif_codes(data):
    # Search for VIF unpack commands in a brute force manner for diagnostic purposes.
    # We look for 0x6C, 0x6D, 0x6E, 0x6F (V4-32, V4-16, V4-8, V3-32)
    # The command is the most significant byte of a 32-bit word.
    found = []
    for pos in range(0, len(data) - 4, 4):
        code = struct.unpack_from('<I', data, pos)[0]
        cmd = (code >> 24) & 0xFF
        num = (code >> 16) & 0xFF
        imm = code & 0xFFFF
        
        # Look specifically for Vertex (0x6C) or UV (0x6A) unpacks with > 0 elements
        if cmd in (0x6A, 0x6C, 0x6D) and num > 0:
            found.append({
                'offset': pos,
                'cmd': cmd,
                'num': num,
                'imm': imm,
                'hex': f"{code:08X}"
            })
    return found

def main():
    original_esf = "workspace/expansion/CHAR.ESF"
    patched_esf = "workspace/FINAL_CHAR_MERGED.ESF"
    target_hash = 0x05AEBA67
    
    print(f"[*] Parsing Golden Master: {original_esf}")
    with open(original_esf, 'rb') as f:
        orig_data = f.read()
    orig_parser = ESFParser(orig_data).parse()
    
    print(f"[*] Parsing Patched File: {patched_esf}")
    with open(patched_esf, 'rb') as f:
        patch_data = f.read()
    patch_parser = ESFParser(patch_data).parse()
    
    orig_model = find_model_node(orig_parser, target_hash)
    patch_model = find_model_node(patch_parser, target_hash)
    
    orig_geom = find_node(orig_model, 0x02610)
    patch_geom = find_node(patch_model, 0x02610)
    
    # We need to scan ALL children of 0x02610, because the payload is split across them.
    orig_vifs = []
    def gather_vifs(node, lst):
        if node['child_count'] == 0 and node['inline_data']:
            vifs = extract_vif_codes(node['inline_data'])
            lst.extend(vifs)
        for child in node['children']:
            gather_vifs(child, lst)
            
    gather_vifs(orig_geom, orig_vifs)
    
    patch_vifs = []
    gather_vifs(patch_geom, patch_vifs)
    
    print("\n================================================================================")
    print(" VIF UNPACK FLAG COMPARISON (First 5 Instructions)")
    print("================================================================================")
    
    print("GOLDEN MASTER (Frontiers Engine Expectation):")
    for i, v in enumerate(orig_vifs[:5]):
        cmd_str = "V4-32 (0x6C)" if v['cmd'] == 0x6C else "V4-16 (0x6D)" if v['cmd'] == 0x6D else "V2-32 (0x6A)" if v['cmd'] == 0x6A else f"0x{v['cmd']:02X}"
        print(f"  [{i}] Code: 0x{v['hex']} | CMD: {cmd_str} | NUM: {v['num']} elements | IMM: 0x{v['imm']:04X}")
        
    print("\nINJECTED PATCH (Vanilla Engine Delivery):")
    for i, v in enumerate(patch_vifs[:5]):
        cmd_str = "V4-32 (0x6C)" if v['cmd'] == 0x6C else "V4-16 (0x6D)" if v['cmd'] == 0x6D else "V2-32 (0x6A)" if v['cmd'] == 0x6A else f"0x{v['cmd']:02X}"
        print(f"  [{i}] Code: 0x{v['hex']} | CMD: {cmd_str} | NUM: {v['num']} elements | IMM: 0x{v['imm']:04X}")
        
    if orig_vifs and patch_vifs:
        if orig_vifs[0]['cmd'] != patch_vifs[0]['cmd']:
            print("\n[!] CRITICAL MISMATCH DETECTED!")
            print(f"    Golden Master expects {hex(orig_vifs[0]['cmd'])} but Patch provides {hex(patch_vifs[0]['cmd'])}!")
            
if __name__ == "__main__":
    main()
