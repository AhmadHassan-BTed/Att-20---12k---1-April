#!/usr/bin/env python3
"""
diagnose_udf_iso.py
====================
Scans both ISO9660 and UDF filesystems for CHAR.ESF references.
Reports exactly what LBA and size each filesystem layer points to.
This proves WHY the game can't see our patched ESF.
"""
import struct, os, sys

ISO_PATH = 'EQOA_Frontiers_Patched.iso'

if not os.path.exists(ISO_PATH):
    print(f"[-] Not found: {ISO_PATH}")
    sys.exit(1)

file_size = os.path.getsize(ISO_PATH)
print(f"[*] ISO size: {file_size:,} bytes ({file_size // 2048} sectors)")
print()

with open(ISO_PATH, 'rb') as f:
    data = f.read()

# ── ISO9660 scan ──────────────────────────────────────────────────────────────
print("=" * 60)
print("  ISO9660 Directory Records for CHAR.ESF")
print("=" * 60)

target = b'CHAR.ESF;1'
idx = 0
found_iso = []
while True:
    idx = data.find(target, idx)
    if idx == -1: break
    # Backtrack to directory record start (filename starts at byte 33 of a DR)
    # DR layout: [0]=len, [1]=xattr_len, [2:6]=LBA_LE, [6:10]=LBA_BE, [10:14]=SIZE_LE
    # filename starts at byte 33; but let's look back 33 bytes
    dr_start = idx - 33
    dr_len = data[dr_start]
    if dr_len < 34: 
        idx += 1
        continue
    lba_le  = struct.unpack_from('<I', data, dr_start + 2)[0]
    lba_be  = struct.unpack_from('>I', data, dr_start + 6)[0]
    size_le = struct.unpack_from('<I', data, dr_start + 10)[0]
    size_be = struct.unpack_from('>I', data, dr_start + 14)[0]
    sector  = dr_start // 2048
    print(f"  Offset: 0x{dr_start:08X}  Sector: {sector}")
    print(f"    LBA  : LE={lba_le}  BE={lba_be}  {'OK' if lba_le==lba_be else 'MISMATCH!'}")
    print(f"    SIZE : LE={size_le:,}  BE={size_be:,}  {'OK' if size_le==size_be else 'MISMATCH!'}")
    found_iso.append((dr_start, lba_le, size_le))
    idx += 1

if not found_iso:
    print("  [!] NO CHAR.ESF ISO9660 DIRECTORY RECORD FOUND!")
print()

# ── UDF scan ─────────────────────────────────────────────────────────────────
print("=" * 60)
print("  UDF File Entries (tag 0x0105) mentioning old CHAR.ESF size")
print("=" * 60)

# The old CHAR.ESF was 148,370,972 bytes (from original ISO)
# The new one has a different size. We search for UDF File Entries
# that have the old OR new size at offset 0x38.
sizes_of_interest = [148370972, 148_370_972]
if found_iso:
    for _, _, sz in found_iso:
        sizes_of_interest.append(sz)

found_udf = []
for sector in range(200, 800):
    off = sector * 2048
    if off + 0x200 > len(data): break
    tag_id = struct.unpack_from('<H', data, off)[0]
    if tag_id != 0x0105: continue  # Not a File Entry
    
    info_len = struct.unpack_from('<Q', data, off + 0x38)[0]
    l_ea     = struct.unpack_from('<I', data, off + 0xA8)[0]
    l_ad     = struct.unpack_from('<I', data, off + 0xAC)[0]
    
    # Only care about big files (>100MB)
    if info_len < 100_000_000: continue
    
    ad_base  = off + 0xB0 + l_ea
    icb_flag = struct.unpack_from('<H', data, off + 0x12 + 18)[0]
    ad_type  = icb_flag & 0x07

    lba_from_ad = None
    if ad_type == 0 and l_ad >= 8:  # short_ad
        ad_len_v = struct.unpack_from('<I', data, ad_base)[0]
        ad_pos_v = struct.unpack_from('<I', data, ad_base + 4)[0]
        lba_from_ad = ad_pos_v
    elif ad_type == 1 and l_ad >= 16:  # long_ad
        ad_lba_v = struct.unpack_from('<I', data, ad_base + 4)[0]
        lba_from_ad = ad_lba_v
    
    print(f"  Sector {sector} (0x{off:X}): File Entry")
    print(f"    Information Length : {info_len:,} bytes")
    print(f"    Logical Blocks     : {struct.unpack_from('<Q', data, off+0x40)[0]}")
    print(f"    L_EA={l_ea}, L_AD={l_ad}, AD type={ad_type}")
    print(f"    Allocation Desc LBA: {lba_from_ad}")
    
    # Is this likely CHAR.ESF?
    for _, iso_lba, iso_size in found_iso:
        if lba_from_ad == iso_lba or info_len == iso_size:
            print(f"    *** THIS IS CHAR.ESF's UDF FILE ENTRY ***")
    found_udf.append((sector, info_len, lba_from_ad))
    print()

if not found_udf:
    print("  [!] No large (>100MB) UDF File Entries found in sectors 200-800!")
    print("      Expanding search range...")
    for sector in range(0, 2000):
        off = sector * 2048
        if off + 0x200 > len(data): break
        tag_id = struct.unpack_from('<H', data, off)[0]
        if tag_id != 0x0105: continue
        info_len = struct.unpack_from('<Q', data, off + 0x38)[0]
        if info_len >= 50_000_000:
            print(f"  Found File Entry at sector {sector}: size={info_len:,}")

print()

# ── Comparison ───────────────────────────────────────────────────────────────
print("=" * 60)
print("  DIAGNOSIS SUMMARY")
print("=" * 60)
if found_iso:
    iso_lba, iso_size = found_iso[0][1], found_iso[0][2]
    print(f"  ISO9660 says CHAR.ESF is at  LBA={iso_lba}, Size={iso_size:,}")
else:
    print("  ISO9660: CHAR.ESF not found!")

if found_udf:
    udf_info = found_udf[0]
    print(f"  UDF     says CHAR.ESF is at  LBA={udf_info[2]}, Size={udf_info[1]:,}")
    if found_iso and (udf_info[2] != found_iso[0][1] or udf_info[1] != found_iso[0][2]):
        print()
        print("  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("  MISMATCH CONFIRMED: UDF and ISO9660 disagree!")
        print("  The PS2 uses UDF -> game reads OLD CHAR.ESF!")
        print("  THIS IS WHY TEXTURES ARE NOT LOADING.")
        print("  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    else:
        print("  ISO9660 and UDF agree (or UDF already patched).")
else:
    print("  UDF: No matching File Entry found.")
