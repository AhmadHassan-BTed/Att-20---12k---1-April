import sys
import os
import struct
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from core.esf_parser import ESFParser

def find_jump_table(filepath):
    print(f"[*] Parsing {filepath}...")
    with open(filepath, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data)
    parser.parse()
    
    # Heuristic: A jump table is usually a leaf node that consists entirely of 32-bit offsets.
    # The offsets would mostly be monotonically increasing and within the file size.
    # Alternatively, the first child of the root node might be the jump table.
    
    first_child = parser.root['children'][0]
    print(f"  [+] Root first child: Type 0x{first_child['type_id']:05X}, Size {first_child['data_size']}, Count {first_child['child_count']}")
    
    if first_child['child_count'] == 0:
        # It's a leaf node. Could this be the jump table?
        jump_table_data = first_child['inline_data']
        print(f"  [+] Found potential Jump Table! Size: {len(jump_table_data)} bytes.")
        return jump_table_data
    
    # If not the first child, let's search for a node that contains large array of pointers
    for child in parser.root['children']:
        if child['child_count'] == 0 and child['data_size'] > 1000:
            # Let's inspect it
            pointers = struct.unpack(f'<{child["data_size"] // 4}I', child['inline_data'][:(child['data_size']//4)*4])
            # Check if monotonically increasing
            valid = True
            for i in range(10):
                if pointers[i] > len(data):
                    valid = False
            if valid:
                print(f"  [+] Found potential Jump Table at Type 0x{child['type_id']:05X}! Size: {child['data_size']}")
                return child['inline_data']
                
    return None

def main():
    original_esf = "workspace/expansion/CHAR.ESF"
    patched_esf = "workspace/FINAL_CHAR_MERGED.ESF"
    
    orig_table = find_jump_table(original_esf)
    patch_table = find_jump_table(patched_esf)
    
    if orig_table and patch_table:
        print("\n================================================================================")
        print(" JUMP TABLE HEX DUMP COMPARISON (First 256 bytes)")
        print("================================================================================")
        print("OFFSET   | ORIGINAL CHAR.ESF                                | PATCHED CHAR.ESF")
        print("--------------------------------------------------------------------------------")
        
        limit = min(256, len(orig_table), len(patch_table))
        for i in range(0, limit, 16):
            orig_hex = " ".join([f"{b:02X}" for b in orig_table[i:i+16]])
            patch_hex = " ".join([f"{b:02X}" for b in patch_table[i:i+16]])
            
            diff_marker = " " if orig_hex == patch_hex else "*"
            print(f"0x{i:04X} {diff_marker} | {orig_hex:<48} | {patch_hex:<48}")
            
        print("\n[*] Analyzing for 0x00000000 NULL pointers in patched table...")
        null_count = 0
        limit = min(len(orig_table), len(patch_table))
        for i in range(0, limit, 4):
            orig_ptr = struct.unpack_from('<I', orig_table, i)[0]
            patch_ptr = struct.unpack_from('<I', patch_table, i)[0]
            if orig_ptr != 0 and patch_ptr == 0:
                null_count += 1
                
        print(f"    -> Found {null_count} instances where original had a pointer but patched has 0x00000000.")

if __name__ == "__main__":
    main()
