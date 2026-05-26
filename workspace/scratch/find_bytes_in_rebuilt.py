import os
import sys

def main():
    merged_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    with open(merged_esf, 'rb') as f:
        data = f.read()
        
    needle = b'\x16\x00\xf5\xe1\x22\x2f\x3e\x2d\x2d\xf0\x00\xe2\x00\xf0\x20\xf1'
    
    sys.path.append('core')
    from esf_parser import ESFParser
    parser = ESFParser(data).parse()
    
    search_start = 0
    while True:
        idx = data.find(needle, search_start)
        if idx == -1:
            break
        print(f"[+] Needle found at offset 0x{idx:X}")
        for entry in parser.pointer_table:
            if entry.offset <= idx < entry.offset + entry.length:
                print(f"  Belongs to asset 0x{entry.asset_id:08X} (offset=0x{entry.offset:X}, len={entry.length})")
                print(f"  Relative offset from asset start: 0x{idx - entry.offset:X}")
                break
        search_start = idx + 1

if __name__ == '__main__':
    main()
