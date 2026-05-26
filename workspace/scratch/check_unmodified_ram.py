import os
import sys

def main():
    frontiers_esf = 'workspace/expansion/CHAR.ESF'
    with open(frontiers_esf, 'rb') as f:
        data = f.read()
        
    sys.path.append('core')
    from esf_parser import ESFParser
    parser = ESFParser(data).parse()
    entry = [e for e in parser.pointer_table if e.asset_id == 0xCD51EF83][0]
    
    # Let's search for the needle in native Frontiers asset 0xCD51EF83
    needle = b'\x16\x00\xf5\xe1\x22\x2f\x3e\x2d\x2d\xf0\x00\xe2\x00\xf0\x20\xf1'
    idx = data.find(needle, entry.offset, entry.offset + entry.length)
    if idx == -1:
        print("[-] Needle NOT found in native Frontiers asset!")
    else:
        print(f"[+] Needle found in native Frontiers at offset 0x{idx:X}")
        print(f"  Relative offset from asset start: 0x{idx - entry.offset:X}")
        
        # Dump around the needle
        start_offset = idx - 4
        chunk = data[start_offset : start_offset + 100]
        print("\n=== EXACT TRANSITION IN NATIVE FRONTIERS CHAR.ESF ===")
        for i in range(0, len(chunk), 16):
            line_chunk = chunk[i:i+16]
            hex_str = " ".join(f"{b:02X}" for b in line_chunk)
            ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in line_chunk)
            print(f"  {start_offset + i:08X}: {hex_str:<47} |{ascii_str}|")

if __name__ == '__main__':
    main()
