import os
import sys
import struct

def main():
    frontiers_esf = 'workspace/expansion/CHAR.ESF'
    rebuilt_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    
    sys.path.append('core')
    from esf_parser import ESFParser
    from verify_injected_models import parse_node
    
    # 1. Native Frontiers
    with open(frontiers_esf, 'rb') as f:
        f_data = f.read()
    f_parser = ESFParser(f_data).parse()
    f_entry = [e for e in f_parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    f_node_bytes = f_data[f_entry.offset : f_entry.offset + f_entry.length]
    f_root, _ = parse_node(f_node_bytes, 0)
    f_b070 = f_root['children'][3]
    f_child3 = f_b070['children'][3]
    
    # 2. Rebuilt
    with open(rebuilt_esf, 'rb') as f:
        r_data = f.read()
    r_parser = ESFParser(r_data).parse()
    r_entry = [e for e in r_parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    r_node_bytes = r_data[r_entry.offset : r_entry.offset + r_entry.length]
    r_root, _ = parse_node(r_node_bytes, 0)
    r_b070 = r_root['children'][3]
    r_child3 = r_b070['children'][3]
    
    print(f"Native Frontiers Child 3: size={f_child3['data_size']}, children={len(f_child3['children'])}")
    print(f"Rebuilt Child 3:          size={r_child3['data_size']}, children={len(r_child3['children'])}")
    
    # Let's extract the raw bytes of Child 3 from both files
    # To find Child 3 offset in f_node_bytes:
    def get_child_bytes_and_offset(root_bytes, root_node, target_type):
        pos = 12
        for idx, child in enumerate(root_node['children']):
            if child['type_id'] == target_type:
                return pos, root_bytes[pos : pos + 12 + child['data_size']]
            pos += 12 + child['data_size']
        return None, None
        
    f_b070_pos, f_b070_bytes = get_child_bytes_and_offset(f_node_bytes, f_root, 0x0B070)
    r_b070_pos, r_b070_bytes = get_child_bytes_and_offset(r_node_bytes, r_root, 0x0B070)
    
    f_c3_pos, f_c3_bytes = get_child_bytes_and_offset(f_b070_bytes, f_b070, 0x0B000) # Wait, get_child_bytes_and_offset gets the first occurrence, which is Child 0.
    # We want child 3 (index 3)
    def get_child_index_bytes(parent_bytes, parent_node, index):
        pos = 12
        for idx, child in enumerate(parent_node['children']):
            if idx == index:
                return pos, parent_bytes[pos : pos + 12 + child['data_size']]
            pos += 12 + child['data_size']
        return None, None
        
    f_c3_pos, f_c3_bytes = get_child_index_bytes(f_b070_bytes, f_b070, 3)
    r_c3_pos, r_c3_bytes = get_child_index_bytes(r_b070_bytes, r_b070, 3)
    
    print(f"Native Frontiers Child 3 physical bytes len: {len(f_c3_bytes)}")
    print(f"Rebuilt Child 3 physical bytes len:          {len(r_c3_bytes)}")
    
    if f_c3_bytes == r_c3_bytes:
        print("[+] Child 3 physical bytes match EXACTLY!")
    else:
        print("[-] Child 3 physical bytes MISMATCH!")
        # Print where they mismatch
        diff_count = 0
        for i in range(min(len(f_c3_bytes), len(r_c3_bytes))):
            if f_c3_bytes[i] != r_c3_bytes[i]:
                print(f"  Mismatch at byte offset 0x{i:X}: Frontiers=0x{f_c3_bytes[i]:02X}, Rebuilt=0x{r_c3_bytes[i]:02X}")
                diff_count += 1
                if diff_count >= 10:
                    print("  ... too many mismatches, truncating")
                    break

if __name__ == '__main__':
    main()
