import os
import sys
import struct
sys.path.append('core')
from esf_parser import ESFParser

def main():
    merged_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    with open(merged_esf, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    
    # 0x0B070 is at entry.offset + 0x15CD1
    b070_offset = entry.offset + 0x15CD1
    print(f"Asset offset in file: 0x{entry.offset:X}")
    print(f"0x0B070 offset in file: 0x{b070_offset:X}")
    
    # Let's inspect around b070_offset + 12 + 9964 + 6976
    transition_offset = b070_offset + 12 + 9964 + 6900
    chunk = data[transition_offset : transition_offset + 200]
    
    print("\n=== RAW HEX DUMP OF TRANSITION IN FINAL_CHAR_MERGED.ESF ===")
    for i in range(0, len(chunk), 16):
        line_chunk = chunk[i:i+16]
        hex_str = " ".join(f"{b:02X}" for b in line_chunk)
        ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in line_chunk)
        print(f"  {transition_offset + i:08X}: {hex_str:<47} |{ascii_str}|")

if __name__ == '__main__':
    main()
