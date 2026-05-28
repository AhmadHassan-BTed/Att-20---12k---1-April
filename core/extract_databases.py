#!/usr/bin/env python3
import mmap
import struct
import os
import sys

def extract_esf_from_iso(iso_path, output_path):
    print(f"[*] Extracting CHAR.ESF from {iso_path} (Baremetal Mode)...")
    with open(iso_path, 'rb') as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        search_str = b'\x0ACHAR.ESF;1'
        idx = mm.find(search_str)
        if idx == -1:
            print(f"[-] Error: CHAR.ESF;1 not found in the ISO directory records of {iso_path}.")
            sys.exit(1)
        
        dr_start = idx - 32
        lba = struct.unpack('<I', mm[dr_start+2:dr_start+6])[0]
        size = struct.unpack('<I', mm[dr_start+10:dr_start+14])[0]
        
        print(f"[*] Found CHAR.ESF at LBA {lba}, size {size:,} bytes")
        
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'wb') as out_f:
            mm.seek(lba * 2048)
            # Read in chunks to avoid memory spikes for huge files
            bytes_left = size
            while bytes_left > 0:
                chunk_size = min(bytes_left, 10 * 1024 * 1024)
                out_f.write(mm.read(chunk_size))
                bytes_left -= chunk_size
                
        mm.close()
    print(f"[+] Successfully extracted {output_path}")

def main():
    vanilla_iso = 'iso/unpatched/EQOA_Vanilla.iso'
    frontiers_iso = 'iso/unpatched/EQOA_Frontiers.iso'
    
    vanilla_out = 'workspace/original/CHAR.ESF'
    frontiers_out = 'workspace/expansion/CHAR.ESF'

    # Check if we already have the ESFs to avoid re-extracting unnecessarily
    if os.path.exists(vanilla_out) and os.path.exists(frontiers_out):
        # We can just skip
        print("[*] Databases already exist in workspace. Skipping extraction.")
        return

    print("=" * 80)
    print("  EQOA BAREMETAL DATABASE EXTRACTOR")
    print("=" * 80)

    if not os.path.exists(vanilla_iso):
        print(f"[-] Error: Could not find Vanilla ISO at {vanilla_iso}")
        print("[-] Please place your original EQOA Vanilla ISO named 'EQOA_Vanilla.iso' in the 'iso/unpatched' folder.")
        sys.exit(1)

    if not os.path.exists(frontiers_iso):
        print(f"[-] Error: Could not find Frontiers ISO at {frontiers_iso}")
        print("[-] Please place your original EQOA Frontiers ISO named 'EQOA_Frontiers.iso' in the 'iso/unpatched' folder.")
        sys.exit(1)

    extract_esf_from_iso(vanilla_iso, vanilla_out)
    extract_esf_from_iso(frontiers_iso, frontiers_out)

if __name__ == '__main__':
    main()
