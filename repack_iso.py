#!/usr/bin/env python3
import os
import sys
import shutil
import struct
import mmap

def repack_iso():
    iso_path = 'EQOA_Frontiers.iso'
    patched_path = 'EQOA_Frontiers_Patched.iso'
    esf_path = 'workspace/FINAL_CHAR_MERGED.ESF'

    if not os.path.exists(iso_path):
        print(f"[-] Error: Could not find original ISO {iso_path}")
        sys.exit(1)
        
    if not os.path.exists(esf_path):
        print(f"[-] Error: Could not find merged ESF {esf_path}")
        sys.exit(1)

    print(f"[*] Copying {iso_path} -> {patched_path} ...")
    shutil.copyfile(iso_path, patched_path)
    
    iso_size = os.path.getsize(patched_path)
    esf_size = os.path.getsize(esf_path)
    
    padding_bytes = 0
    if iso_size % 2048 != 0:
        padding_bytes = 2048 - (iso_size % 2048)
        
    new_lba = (iso_size + padding_bytes) // 2048
    new_total_size = iso_size + padding_bytes + esf_size
    
    print(f"[*] Appending FINAL_CHAR_MERGED.ESF to the end of Patched ISO...")
    print(f"    Original ISO Size: {iso_size:,} bytes")
    print(f"    Alignment Padding: {padding_bytes} bytes")
    print(f"    Appended ESF Size: {esf_size:,} bytes")
    print(f"    New Target LBA:    {new_lba}")
    
    with open(patched_path, 'r+b') as f:
        f.seek(0, 2)
        if padding_bytes > 0:
            f.write(b'\x00' * padding_bytes)
            
        with open(esf_path, 'rb') as esf_f:
            for chunk in iter(lambda: esf_f.read(4 * 1024 * 1024), b""):
                f.write(chunk)
                
        # 1. Enforce 2048-Byte EOF Padding
        filesize = f.tell()
        remainder = filesize % 2048
        if remainder != 0:
            padding_needed = 2048 - remainder
            f.write(b'\x00' * padding_needed)
            final_aligned_filesize = filesize + padding_needed
            print(f"[*] Appended {padding_needed} bytes of null padding to sector-align the EOF.")
        else:
            final_aligned_filesize = filesize

                
    print("[*] Performing surgical LBA patch on ISO 9660 Directory Records...")
    records_patched = 0
    
    with open(patched_path, 'r+b') as f:
        mm = mmap.mmap(f.fileno(), 0)
        
        # Search for ISO 9660 Directory Record of CHAR.ESF
        # 32 bytes before the file identifier length (0x0A) and name (CHAR.ESF;1)
        search_str = b'\x0ACHAR.ESF;1'
        idx = 0
        
        while True:
            idx = mm.find(search_str, idx)
            if idx == -1:
                break
                
            dr_start = idx - 32
            
            # Read current LBA to verify structure
            lba_le = struct.unpack('<I', mm[dr_start+2:dr_start+6])[0]
            lba_be = struct.unpack('>I', mm[dr_start+6:dr_start+10])[0]
            
            if lba_le == lba_be:
                old_size = struct.unpack('<I', mm[dr_start+10:dr_start+14])[0]
                print(f"  [+] Found valid Directory Record at offset 0x{dr_start:X}")
                print(f"      Old LBA:  {lba_le} | Old Size: {old_size:,}")
                
                # Overwrite LBA
                mm[dr_start+2:dr_start+6] = struct.pack('<I', new_lba)
                mm[dr_start+6:dr_start+10] = struct.pack('>I', new_lba)
                
                # Overwrite Size
                mm[dr_start+10:dr_start+14] = struct.pack('<I', esf_size)
                mm[dr_start+14:dr_start+18] = struct.pack('>I', esf_size)
                
                print(f"      New LBA:  {new_lba} | New Size: {esf_size:,}")
                records_patched += 1
                
            idx += len(search_str)

        if records_patched == 0:
            print("[-] Warning: No Directory Records patched!")

        print("[*] Updating Primary Volume Descriptor (PVD)...")
        pvd_offset = 16 * 2048
        # FIX: Check 6 bytes for the PVD magic signature instead of 5
        if mm[pvd_offset:pvd_offset+6] == b'\x01CD001':
            vol_size_le = struct.unpack('<I', mm[pvd_offset+80:pvd_offset+84])[0]
            # 2. Update the Primary Volume Descriptor (PVD)
            total_sectors = final_aligned_filesize // 2048
            print(f"  [+] PVD located at offset 0x{pvd_offset:X}")
            print(f"      Old Volume Space Size: {vol_size_le} sectors")
            
            # Write 32-bit Little-Endian and 32-bit Big-Endian sizes
            mm[pvd_offset+80:pvd_offset+84] = struct.pack('<I', total_sectors)
            mm[pvd_offset+84:pvd_offset+88] = struct.pack('>I', total_sectors)
            print(f"      New Volume Space Size: {total_sectors} sectors")
        else:
            print("[-] Warning: PVD magic signature not found at sector 16.")
            
        mm.close()

    print("\n[+] Repacking Complete! Generated EQOA_Frontiers_Patched.iso successfully.")

if __name__ == '__main__':
    repack_iso()
