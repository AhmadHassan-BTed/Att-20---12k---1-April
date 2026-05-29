#!/usr/bin/env python3
import struct, os

def main():
    # If merged-assets/data/CHAR.ESF exists, that is the expected size. Otherwise, FINAL_CHAR_MERGED.ESF.
    assets_esf = 'merged-assets/data/CHAR.ESF'
    fallback_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    
    if os.path.exists(assets_esf):
        expected_size = os.path.getsize(assets_esf)
        label = "Placed Asset CHAR.ESF"
    elif os.path.exists(fallback_esf):
        expected_size = os.path.getsize(fallback_esf)
        label = "Compiled FINAL_CHAR_MERGED.ESF"
    else:
        print("[-] Error: No reference ESF found to compare sizes.")
        return

    iso_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
    if not os.path.exists(iso_path):
        print(f"[-] Error: {iso_path} not found.")
        return

    with open(iso_path, 'rb') as f:
        # Read allocation descriptor
        f.seek(0xA8934)
        ad = f.read(8)
        
        sz = struct.unpack('<I', ad[0:4])[0]
        lb = struct.unpack('<I', ad[4:8])[0]
        phys = lb + 278  # partition offset = 278
        
        print("== UDF Allocation Descriptor ==")
        print(f"  Raw bytes    : {ad.hex()}")
        print(f"  Size (LE32)  : {sz:,}  (expected {expected_size:,} for {label}) -> {'PASS' if sz==expected_size else 'FAIL'}")
        print(f"  LogLBA(LE32) : {lb}  (expected >1490000) -> {'PASS' if lb > 1490000 else 'FAIL'}")
        print(f"  PhysLBA      : {phys}  (expected >1490000) -> {'PASS' if phys > 1490000 else 'FAIL'}")
        
        # Read UDF Information Length
        f.seek(0xA8838)
        il = struct.unpack('<Q', f.read(8))[0]
        print()
        print("== UDF Information Length ==")
        print(f"  {il:,}  (expected {expected_size:,}) -> {'PASS' if il==expected_size else 'FAIL'}")
        
        # Read ISO9660 directory record
        f.seek(0)
        data = f.read(1024*1024)  # first 1MB should contain directory
        idx = data.find(b'CHAR.ESF;1')
        print()
        print("== ISO9660 Directory Record ==")
        if idx != -1:
            dr = idx - 33
            iso_lba  = struct.unpack_from('<I', data, dr+2)[0]
            iso_size = struct.unpack_from('<I', data, dr+10)[0]
            print(f"  LBA  : {iso_lba}  (expected {phys}) -> {'PASS' if iso_lba==phys else 'FAIL'}")
            print(f"  Size : {iso_size:,}  (expected {expected_size:,}) -> {'PASS' if iso_size==expected_size else 'FAIL'}")
        
        # Verify CHAR.ESF data at patched LBA
        f.seek(phys * 2048)
        magic = f.read(4)
        print()
        print(f"== CHAR.ESF data at LBA {phys} ==")
        print(f"  Magic: {magic.hex()}  (expected 464a424f = FJBO) -> {'PASS' if magic==b'FJBO' else 'FAIL'}")
        
    print()
    print("All checks done. If all PASS above, the ISO is correctly patched.")

if __name__ == '__main__':
    main()
