import os
import sys
import struct

def main():
    merged_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    with open(merged_esf, 'rb') as f:
        data = f.read()
        
    # The needle was found at 0x7F93109
    start_offset = 0x7F93109 - 4
    chunk = data[start_offset : start_offset + 100]
    
    print("\n=== EXACT TRANSITION IN FINAL_CHAR_MERGED.ESF ===")
    for i in range(0, len(chunk), 16):
        line_chunk = chunk[i:i+16]
        hex_str = " ".join(f"{b:02X}" for b in line_chunk)
        ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in line_chunk)
        print(f"  {start_offset + i:08X}: {hex_str:<47} |{ascii_str}|")

if __name__ == '__main__':
    main()
