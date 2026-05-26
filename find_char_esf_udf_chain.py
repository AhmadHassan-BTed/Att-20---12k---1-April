#!/usr/bin/env python3
"""
find_char_esf_udf_chain.py
===========================
Comprehensive scan to find the exact UDF chain for CHAR.ESF:
1. Find the File Identifier Descriptor (FID) that references CHAR.ESF
2. Follow the ICB to the File Entry
3. Read the Allocation Descriptors from the File Entry
4. Confirm the LBA that the PS2 IOP would use to read CHAR.ESF

This tells us EXACTLY what needs to be patched.
"""
import struct, os, sys

ISO_PATH = 'EQOA_Frontiers_Patched.iso'

KNOWN_OLD_SIZE = 148_370_972  # In the UDF FE we found at sector 337
NEW_LBA        = 1492368
NEW_SIZE       = os.path.getsize('workspace/FINAL_CHAR_MERGED.ESF')

print(f"ISO: {ISO_PATH}")
print(f"New ESF size: {NEW_SIZE:,}")
print()

with open(ISO_PATH, 'rb') as f:
    data = f.read()

iso_size = len(data)

# ── Strategy 1: Find File Entry by old size (0x08D7F61C) ──────────────────────
print("=" * 60)
print("Strategy 1: Find UDF File Entries with size = 148,370,972")
print("=" * 60)
old_size_8le = struct.pack('<Q', KNOWN_OLD_SIZE)

found_fes = []
for sector in range(0, min(3000, iso_size // 2048)):
    off = sector * 2048
    if off + 0x200 > iso_size: break
    tag_id = struct.unpack_from('<H', data, off)[0]
    if tag_id != 0x0105: continue
    info_len = struct.unpack_from('<Q', data, off + 0x38)[0]
    if info_len == KNOWN_OLD_SIZE:
        found_fes.append(sector)
        l_ea = struct.unpack_from('<I', data, off + 0xA8)[0]
        l_ad = struct.unpack_from('<I', data, off + 0xAC)[0]
        icb_flag = struct.unpack_from('<H', data, off + 0x12 + 18)[0]
        ad_type = icb_flag & 0x07
        ad_start = off + 0xB0 + l_ea
        print(f"  File Entry at sector {sector} (0x{off:X}):")
        print(f"    size={info_len:,}, L_EA={l_ea}, L_AD={l_ad}, AD_type={ad_type}")
        
        # Dump allocation descriptors
        print(f"    Allocation Descriptors ({l_ad} bytes at 0x{ad_start:X}):")
        print(f"    Raw: {data[ad_start:ad_start+32].hex()}")
        
        if ad_type == 0 and l_ad >= 8:  # short_ad
            for i in range(0, l_ad, 8):
                if i + 8 > l_ad: break
                ext_len = struct.unpack_from('<I', data, ad_start + i)[0]
                ext_pos = struct.unpack_from('<I', data, ad_start + i + 4)[0]
                print(f"    short_ad[{i//8}]: len={ext_len & 0x3FFFFFFF:,}, type={ext_len>>30}, pos(LBA)={ext_pos}")
        elif ad_type == 1 and l_ad >= 16:  # long_ad
            for i in range(0, l_ad, 16):
                if i + 16 > l_ad: break
                ext_len = struct.unpack_from('<I', data, ad_start + i)[0]
                ext_lba = struct.unpack_from('<I', data, ad_start + i + 4)[0]
                ext_ptn = struct.unpack_from('<H', data, ad_start + i + 8)[0]
                print(f"    long_ad[{i//16}]: len={ext_len & 0x3FFFFFFF:,}, type={ext_len>>30}, LBA={ext_lba}, part={ext_ptn}")
        elif ad_type == 7:
            print(f"    AD type 7: data stored inline (no LBA pointer)")
            # Print the full ICB tag field for type 7
            print(f"    ICB Tag: {data[off+0x10:off+0x24].hex()}")
        else:
            print(f"    Unhandled AD type {ad_type}, dumping raw L_AD bytes:")
            print(f"    {data[ad_start:ad_start+l_ad].hex()}")

if not found_fes:
    print("  None found!")

print()

# ── Strategy 2: Scan all File Entries, find those with LBA near 3578 ──────────
print("=" * 60)
print("Strategy 2: Find File Entries with Allocation LBA near original")
print("  (LBA 3578 = original CHAR.ESF location)")
print("=" * 60)

for sector in range(0, min(3000, iso_size // 2048)):
    off = sector * 2048
    if off + 0x200 > iso_size: break
    tag_id = struct.unpack_from('<H', data, off)[0]
    if tag_id != 0x0105: continue  # Not a File Entry
    
    l_ea = struct.unpack_from('<I', data, off + 0xA8)[0]
    l_ad = struct.unpack_from('<I', data, off + 0xAC)[0]
    icb_flag = struct.unpack_from('<H', data, off + 0x12 + 18)[0]
    ad_type = icb_flag & 0x07
    ad_start = off + 0xB0 + l_ea
    
    if ad_type == 0 and l_ad >= 8:
        ext_pos = struct.unpack_from('<I', data, ad_start + 4)[0]
        if 3000 <= ext_pos <= 4000:
            info_len = struct.unpack_from('<Q', data, off + 0x38)[0]
            print(f"  File Entry at sector {sector}: LBA={ext_pos}, size={info_len:,}")
    elif ad_type == 1 and l_ad >= 16:
        ext_lba = struct.unpack_from('<I', data, ad_start + 4)[0]
        if 3000 <= ext_lba <= 4000:
            info_len = struct.unpack_from('<Q', data, off + 0x38)[0]
            print(f"  File Entry at sector {sector}: LBA={ext_lba}, size={info_len:,}")

print()

# ── Strategy 3: Direct binary search for old LBA 3578 in the UDF area ─────────
print("=" * 60)
print("Strategy 3: Direct binary search for LBA 3578 in sectors 200-800")
print("=" * 60)
target_le = struct.pack('<I', 3578)
target_be = struct.pack('>I', 3578)

search_region_start = 200 * 2048
search_region_end   = min(800 * 2048, iso_size)

pos = search_region_start
found_count = 0
while pos < search_region_end:
    p = data.find(target_le, pos, search_region_end)
    if p == -1: break
    sector = p // 2048
    off_in_sector = p % 2048
    context = data[max(0,p-8):p+12].hex()
    print(f"  0x{p:08X} (sector {sector}, off 0x{off_in_sector:03X}): LE=3578  ctx: {context}")
    found_count += 1
    pos = p + 1
    if found_count > 20: 
        print("  [too many results, truncating]")
        break

pos = search_region_start
while pos < search_region_end:
    p = data.find(target_be, pos, search_region_end)
    if p == -1: break
    sector = p // 2048
    off_in_sector = p % 2048
    context = data[max(0,p-8):p+12].hex()
    print(f"  0x{p:08X} (sector {sector}, off 0x{off_in_sector:03X}): BE=3578  ctx: {context}")
    found_count += 1
    pos = p + 1
    if found_count > 20:
        print("  [truncating]")
        break

if found_count == 0:
    print("  LBA 3578 not found in UDF region!")

print()

# ── Strategy 4: Scan the ISO9660 Path Table area for pointers ─────────────────
print("=" * 60)
print("Strategy 4: What does the ISO tell us about data at LBA 3578?")
print("=" * 60)
char_esf_sector = 3578 * 2048
if char_esf_sector + 32 < iso_size:
    magic = data[char_esf_sector:char_esf_sector+16]
    print(f"  Data at LBA 3578 (original CHAR.ESF start): {magic.hex()}")
    print(f"  ASCII: {''.join(chr(b) if 32<=b<127 else '.' for b in magic)}")
else:
    print(f"  LBA 3578 is beyond EOF of patched ISO (ISO may have been remapped)")

new_char_esf_off = NEW_LBA * 2048
if new_char_esf_off + 32 < iso_size:
    magic2 = data[new_char_esf_off:new_char_esf_off+16]
    print(f"  Data at NEW LBA {NEW_LBA}: {magic2.hex()}")
    print(f"  ASCII: {''.join(chr(b) if 32<=b<127 else '.' for b in magic2)}")
