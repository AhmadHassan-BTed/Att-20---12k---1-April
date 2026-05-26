#!/usr/bin/env python3
import struct, os

esf_path = 'workspace/FINAL_CHAR_MERGED.ESF'
expected_size = os.path.getsize(esf_path)

with open('iso/patched/EQOA_Frontiers_Patched.iso', 'rb') as f:
    # Read allocation descriptor
    f.seek(0xA8934)
    ad = f.read(8)
    
    sz = struct.unpack('<I', ad[0:4])[0]
    lb = struct.unpack('<I', ad[4:8])[0]
    phys = lb + 278  # partition offset = 278
    
    print("== UDF Allocation Descriptor ==")
    print(f"  Raw bytes    : {ad.hex()}")
    print(f"  Size (LE32)  : {sz:,}  (expected {expected_size:,}) -> {'PASS' if sz==expected_size else 'FAIL'}")
    print(f"  LogLBA(LE32) : {lb}  (expected 1492090) -> {'PASS' if lb==1492090 else 'FAIL'}")
    print(f"  PhysLBA      : {phys}  (expected 1492368) -> {'PASS' if phys==1492368 else 'FAIL'}")
    
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
        print(f"  LBA  : {iso_lba}  (expected 1492368) -> {'PASS' if iso_lba==1492368 else 'FAIL'}")
        print(f"  Size : {iso_size:,}  (expected {expected_size:,}) -> {'PASS' if iso_size==expected_size else 'FAIL'}")
    
    # Verify CHAR.ESF data at patched LBA
    f.seek(1492368 * 2048)
    magic = f.read(4)
    print()
    print("== CHAR.ESF data at LBA 1492368 ==")
    print(f"  Magic: {magic.hex()}  (expected 464a424f = FJBO) -> {'PASS' if magic==b'FJBO' else 'FAIL'}")
    
print()
print("All checks done. If all PASS above, the ISO is correctly patched.")
