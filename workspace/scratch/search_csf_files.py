import sys
import struct
import os

sys.path.append(r't:\Att 20 - 12k - 1 April')
from core.verify_final_iso import verify_final_iso # dummy

# Let's search the ISO for all file names ending in .CSF
def search_udf_filenames(iso_path):
    print(f"[*] Scanning UDF structures in {iso_path} for .CSF files...")
    if not os.path.exists(iso_path):
        print("[-] ISO not found.")
        return
        
    with open(iso_path, 'rb') as f:
        data = f.read(20 * 1024 * 1024) # Scan first 20MB of UDF directory descriptors
        
    idx = 0
    while True:
        idx = data.find(b'.CSF', idx)
        if idx == -1:
            break
        # Extract filename (UDF or ISO9660)
        # Scan backward for a non-printable character or space
        start = idx
        while start > 0 and (32 <= data[start-1] < 127 or data[start-1] in (95, 45)): # A-Z, 0-9, _, -
            start -= 1
        filename = data[start:idx+4].decode('utf-8', errors='replace')
        if len(filename) > 3:
            print(f"  [+] Found CSF file descriptor: {filename} at offset 0x{start:X}")
        idx += 4

def main():
    iso_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
    search_udf_filenames(iso_path)

if __name__ == '__main__':
    main()
