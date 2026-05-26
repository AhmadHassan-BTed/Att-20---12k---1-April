#!/usr/bin/env python3
"""
deep_scan_udf_fe.py
===================
Deep scan the entire UDF File Entry sector to find where LBA 3578 is stored.
Also dumps the ENTIRE sector so we can find the allocation descriptor chain.
"""
import struct, os, sys

ISO_PATH = 'EQOA_Frontiers_Patched.iso'
UDF_FE_OFF = 337 * 2048  # 0xA8800

OLD_LBA  = 3578        # 0x00000DFA
OLD_SIZE = 148_370_972 # 0x08D7F61C

with open(ISO_PATH, 'rb') as f:
    f.seek(UDF_FE_OFF)
    sector = f.read(2048)

print(f"[*] Full UDF File Entry sector (sector 337) scan:")
print(f"    Looking for LBA {OLD_LBA} = 0x{OLD_LBA:08X}")
print()

# Dump the whole sector in hex
print("[*] FULL SECTOR HEX DUMP:")
for row in range(0, 2048, 16):
    chunk = sector[row:row+16]
    if all(b == 0 for b in chunk):
        continue  # skip zero rows
    hex_part = ' '.join(f'{b:02X}' for b in chunk)
    asc_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
    print(f"  {row:04X}: {hex_part:<48}  {asc_part}")

print()
print("[*] Scanning for LBA values (3578 = 0x0DFA):")
# 4-byte LE
target_le = struct.pack('<I', OLD_LBA)
pos = 0
while True:
    pos = sector.find(target_le, pos)
    if pos == -1: break
    print(f"  Found 4LE at offset 0x{pos:04X}: context: {sector[max(0,pos-4):pos+8].hex()}")
    pos += 1

# 4-byte BE  
target_be = struct.pack('>I', OLD_LBA)
pos = 0
while True:
    pos = sector.find(target_be, pos)
    if pos == -1: break
    print(f"  Found 4BE at offset 0x{pos:04X}: context: {sector[max(0,pos-4):pos+8].hex()}")
    pos += 1

# 2-byte LE
target_2le = struct.pack('<H', OLD_LBA & 0xFFFF)
pos = 0
while True:
    pos = sector.find(target_2le, pos)
    if pos == -1: break
    print(f"  Found 2LE (0x{OLD_LBA&0xFFFF:04X}) at offset 0x{pos:04X}: context: {sector[max(0,pos-4):pos+6].hex()}")
    pos += 1

print()

# Also scan the NEXT sector (sector 338) in case the AD spills over  
print("[*] Also scanning next sector (338) for LBA 3578...")
with open(ISO_PATH, 'rb') as f:
    f.seek(338 * 2048)
    next_sector = f.read(2048)

pos = 0
while True:
    pos = next_sector.find(target_le, pos)
    if pos == -1: break
    print(f"  Sector 338 offset 0x{pos:04X}: context: {next_sector[max(0,pos-4):pos+8].hex()}")
    pos += 1

print()

# Look for the UDF File Identifier Descriptor (tag 0x0102) for CHAR.ESF
print("[*] Searching for UDF File Identifier Descriptors (FID, tag 0x0102) containing 'CHAR'...")
with open(ISO_PATH, 'rb') as f:
    data = f.read()

char_bytes = b'C\x00H\x00A\x00R\x00'  # Unicode
pos = 0
while True:
    pos = data.find(char_bytes, pos)
    if pos == -1: break
    # Check if this is in a FID - look for tag 0x0102 within 64 bytes back
    region = data[max(0, pos-100):pos+20]
    for i in range(len(region)-2):
        if struct.unpack_from('<H', region, i)[0] == 0x0102:
            abs_off = max(0, pos-100) + i
            sector = abs_off // 2048
            print(f"  FID at sector {sector} offset 0x{abs_off:X}: contains CHAR")
            # Print the ICB allocation extent from the FID
            # FID structure: tag(16) + file_version_number(2) + file_chars(1) + 
            #                len_fi(1) + icb(16 for long_ad: 4+8+2+2) + len_iu(2) + iu + fi
            # icb starts at FID offset 18: ICB is a long_ad = 4 bytes extLen + 4 bytes LBA + 2 bytes partition + 2 bytes impl
            fid_base = abs_off
            icb_lba = struct.unpack_from('<I', data, fid_base + 20)[0]
            icb_part = struct.unpack_from('<H', data, fid_base + 24)[0]
            print(f"    ICB LBA: {icb_lba}, Partition: {icb_part}")
            break
    pos += 1
