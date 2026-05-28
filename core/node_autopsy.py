import os
import sys
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def find_model_node(parser, target_hash):
    # Find the 0x0A010 Model Container
    for child in parser.root['children']:
        if child['type_id'] == 0x0A010:
            for model_node in child['children']:
                # The model_node is typically 0x72700. Let's find the hash inside it.
                h = parser._find_hash_in_subtree(model_node)
                if h == target_hash:
                    return model_node
    return None

def extract_raw_node(node, data):
    # ESF parser gives us offset and data_size.
    # The total size of the node is 12 bytes (header) + data_size
    start = node['offset']
    end = start + 12 + node['data_size']
    return data[start:end]

def main():
    original_esf = "workspace/expansion/CHAR.ESF"
    patched_esf = "workspace/FINAL_CHAR_MERGED.ESF"
    target_hash = 0x05AEBA67  # Ogre model
    
    print(f"[*] Parsing Golden Master: {original_esf}")
    with open(original_esf, 'rb') as f:
        orig_data = f.read()
    orig_parser = ESFParser(orig_data)
    orig_parser.parse()
    
    print(f"[*] Parsing Patched File: {patched_esf}")
    with open(patched_esf, 'rb') as f:
        patch_data = f.read()
    patch_parser = ESFParser(patch_data)
    patch_parser.parse()
    
    orig_node = find_model_node(orig_parser, target_hash)
    patch_node = find_model_node(patch_parser, target_hash)
    
    if not orig_node:
        print("[-] Original node not found!")
        sys.exit(1)
    if not patch_node:
        print("[-] Patched node not found!")
        sys.exit(1)
        
    print(f"  [+] Original Node Type: 0x{orig_node['type_id']:05X}, Size: {orig_node['data_size']}")
    print(f"  [+] Patched Node Type:  0x{patch_node['type_id']:05X}, Size: {patch_node['data_size']}")
    
    orig_bin = extract_raw_node(orig_node, orig_data)
    patch_bin = extract_raw_node(patch_node, patch_data)
    
    print("\n================================================================================")
    print(" 0x72700 NODE AUTOPSY: HEADER HEX DUMP (First 128 bytes)")
    print("================================================================================")
    print("OFFSET   | GOLDEN MASTER (ORIGINAL)                         | INJECTED (PATCHED)")
    print("--------------------------------------------------------------------------------")
    
    limit = min(128, len(orig_bin), len(patch_bin))
    for i in range(0, limit, 16):
        orig_hex = " ".join([f"{b:02X}" for b in orig_bin[i:i+16]])
        patch_hex = " ".join([f"{b:02X}" for b in patch_bin[i:i+16]])
        
        diff_marker = " " if orig_hex == patch_hex else "*"
        print(f"0x{i:04X} {diff_marker} | {orig_hex:<48} | {patch_hex:<48}")
        
    print("\n[*] Analysis Complete.")

if __name__ == "__main__":
    main()
