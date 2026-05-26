import struct
import os

def scan_file(filepath, label):
    print(f"\n=== Scanning flat nodes of {label}: {filepath} ===")
    with open(filepath, 'rb') as f:
        data = f.read()
        
    pos = 32
    count = 0
    while pos < len(data):
        if pos + 12 > len(data):
            break
        type_id, data_size, child_count = struct.unpack_from('<III', data, pos)
        
        # Check if this looks like a valid node header
        # data_size should be smaller than remaining file size, child_count should be small
        if type_id in (0x01000, 0x01100, 0x11110, 0x11111) and pos + 12 + data_size <= len(data) and child_count < 100:
            print(f"  Node {count}: pos=0x{pos:X}, type=0x{type_id:05X}, size={data_size:,} B, children={child_count}")
            pos += 12 + data_size
            count += 1
        else:
            pos += 4
            
    print(f"Total flat nodes parsed in {label}: {count}")
    print(f"Final pos: 0x{pos:X} (file size = 0x{len(data):X}, remaining = {len(data) - pos} bytes)")

scan_file('workspace/CHARCUST_Original.ESF', 'Original CHARCUST')
scan_file('workspace/CHARCUST_Frontiers.ESF', 'Frontiers CHARCUST')
