#!/usr/bin/env python3
"""
patch_udf_char_esf.py
=====================
Patches BOTH the ISO9660 directory record AND the UDF File Entry for CHAR.ESF
in EQOA_Frontiers_Patched.iso so the PS2 IOP can actually find our new data.

The PS2 IOP uses the UDF filesystem layer to load files at runtime, NOT ISO9660.
All previous patches only updated the ISO9660 directory record, which is why
the game never loaded the new CHAR.ESF — it kept reading the old one via UDF.

UDF File Entry for CHAR.ESF is at Sector 337, offset 0xA8800 in the ISO.
Within the File Entry:
  - Information Length (file size) is at offset 0x38 (8 bytes, little-endian)
  - Allocation Descriptors follow the Extended Attributes at a computed offset.
    For short_ad: 4 bytes length + 4 bytes position (LBA)
"""

import struct
import os
import sys
import mmap

ISO_PATH   = 'EQOA_Frontiers_Patched.iso'
ESF_PATH   = 'workspace/FINAL_CHAR_MERGED.ESF'

# Known from repack_iso.py logs:
NEW_LBA    = 1492368
NEW_SIZE   = os.path.getsize(ESF_PATH)

# UDF File Entry sector for CHAR.ESF (found via binary scan)
UDF_FE_SECTOR = 337
UDF_FE_OFFSET = UDF_FE_SECTOR * 2048  # = 0xA8800

print("=" * 70)
print("  UDF + ISO9660 DUAL-FILESYSTEM CHAR.ESF SURGICAL PATCHER")
print("=" * 70)
print(f"  New ESF Size : {NEW_SIZE:,} bytes")
print(f"  New LBA      : {NEW_LBA} (0x{NEW_LBA:08X})")
print(f"  UDF FE Sector: {UDF_FE_SECTOR} (offset 0x{UDF_FE_OFFSET:X})")
print()

if not os.path.exists(ISO_PATH):
    print(f"[-] ISO not found: {ISO_PATH}")
    sys.exit(1)

if not os.path.exists(ESF_PATH):
    print(f"[-] ESF not found: {ESF_PATH}")
    sys.exit(1)

with open(ISO_PATH, 'r+b') as f:
    mm = mmap.mmap(f.fileno(), 0)

    # ─────────────────────────────────────────────────────────────────
    # 1. VERIFY UDF File Entry tag at sector 337
    # ─────────────────────────────────────────────────────────────────
    fe_base = UDF_FE_OFFSET
    tag_id  = struct.unpack_from('<H', mm, fe_base)[0]
    print(f"[*] UDF descriptor tag ID at sector {UDF_FE_SECTOR}: 0x{tag_id:04X}")

    # UDF descriptor tag ID 0x0105 = File Entry
    if tag_id != 0x0105:
        print(f"[!] WARNING: Expected File Entry tag 0x0105, got 0x{tag_id:04X}")
        print("    The UDF File Entry may not be at the expected sector.")
        print("    Searching for alternate UDF File Entry location...")
        
        # Search for the File Entry by looking for the old size (8 bytes LE at offset 0x38 from FE start)
        old_size_bytes = struct.pack('<Q', 148370972)
        found = False
        for sector in range(260, 500):
            off = sector * 2048
            t   = struct.unpack_from('<H', mm, off)[0]
            if t == 0x0105:  # File Entry
                sz = struct.unpack_from('<Q', mm, off + 0x38)[0]
                if sz == 148370972:
                    fe_base = off
                    print(f"[+] Found alternate UDF File Entry at sector {sector} (offset 0x{off:X})")
                    found = True
                    break
        if not found:
            print("[-] Could not find UDF File Entry. Only patching ISO9660.")
    else:
        print(f"[+] Confirmed UDF File Entry at sector {UDF_FE_SECTOR}.")

    # ─────────────────────────────────────────────────────────────────
    # 2. READ current UDF File Entry fields
    # ─────────────────────────────────────────────────────────────────
    # UDF File Entry structure (ECMA-167, Section 14.9):
    #   +0x00  Descriptor Tag (16 bytes)
    #   +0x10  ICB Tag (20 bytes)
    #   +0x24  UID (4 bytes)
    #   +0x28  GID (4 bytes)
    #   +0x2C  Permissions (4 bytes)
    #   +0x30  File Link Count (2 bytes)
    #   +0x32  Record Format (1 byte)
    #   +0x33  Record Display Attrs (1 byte)
    #   +0x34  Record Length (4 bytes)
    #   +0x38  Information Length (8 bytes) <-- FILE SIZE
    #   +0x40  Logical Blocks Recorded (8 bytes) <-- BLOCK COUNT
    #   ...
    #   +0xA8  L_EA: Length of Extended Attributes (4 bytes)
    #   +0xAC  L_AD: Length of Allocation Descriptors (4 bytes)
    #   +0xB0  Extended Attributes (L_EA bytes)
    #   +0xB0+L_EA: Allocation Descriptors (L_AD bytes)

    info_len_offset = fe_base + 0x38
    blocks_offset   = fe_base + 0x40
    lea_offset      = fe_base + 0xA8
    lad_offset      = fe_base + 0xAC

    old_info_len = struct.unpack_from('<Q', mm, info_len_offset)[0]
    old_blocks   = struct.unpack_from('<Q', mm, blocks_offset)[0]
    l_ea         = struct.unpack_from('<I', mm, lea_offset)[0]
    l_ad         = struct.unpack_from('<I', mm, lad_offset)[0]
    
    ad_start     = fe_base + 0xB0 + l_ea

    print(f"\n[*] UDF File Entry current state:")
    print(f"    Information Length : {old_info_len:,} bytes")
    print(f"    Logical Blocks Rec : {old_blocks}")
    print(f"    L_EA               : {l_ea}")
    print(f"    L_AD               : {l_ad}")
    print(f"    Alloc Descs start  : 0x{ad_start:X}")

    # ─────────────────────────────────────────────────────────────────
    # 3. PATCH UDF File Entry: Information Length + Logical Blocks
    # ─────────────────────────────────────────────────────────────────
    new_blocks = (NEW_SIZE + 2047) // 2048

    print(f"\n[*] Patching UDF File Entry...")
    print(f"    Information Length : {old_info_len:,} -> {NEW_SIZE:,}")
    print(f"    Logical Blocks Rec : {old_blocks} -> {new_blocks}")

    mm[info_len_offset : info_len_offset + 8] = struct.pack('<Q', NEW_SIZE)
    mm[blocks_offset   : blocks_offset   + 8] = struct.pack('<Q', new_blocks)

    # ─────────────────────────────────────────────────────────────────
    # 4. PATCH UDF Allocation Descriptors (short_ad or long_ad)
    # ─────────────────────────────────────────────────────────────────
    # ICB Tag flags bits [0:2] determine AD type:
    #   0 = short_ad (8 bytes: 4 len + 4 pos)
    #   1 = long_ad  (16 bytes: 4 len + 8 bytes lb_addr)
    #   3 = extended_ad
    icb_flags = struct.unpack_from('<H', mm, fe_base + 0x12 + 18)[0]
    ad_type   = icb_flags & 0x0007
    print(f"    ICB AD Type        : {ad_type} ({'short_ad' if ad_type == 0 else 'long_ad' if ad_type == 1 else 'unknown'})")

    if ad_type == 0:
        # short_ad: uint32 Extent Length, uint32 Extent Position (LBA)
        # There may be multiple ADs if file spans multiple extents
        # We expect 1 AD for CHAR.ESF
        ad_len_val = struct.unpack_from('<I', mm, ad_start)[0]
        ad_pos_val = struct.unpack_from('<I', mm, ad_start + 4)[0]
        print(f"    AD[0] ExtLen       : {ad_len_val & 0x3FFFFFFF:,} (type {ad_len_val >> 30})")
        print(f"    AD[0] ExtPos (LBA) : {ad_pos_val}")

        # Patch: keep extent type bits [30:31] from original, update length and position
        extent_type_bits = (ad_len_val >> 30) << 30
        new_ad_len = extent_type_bits | (NEW_SIZE & 0x3FFFFFFF)
        mm[ad_start     : ad_start + 4] = struct.pack('<I', new_ad_len)
        mm[ad_start + 4 : ad_start + 8] = struct.pack('<I', NEW_LBA)
        print(f"    -> Patched: ExtLen={new_ad_len & 0x3FFFFFFF:,} ExtPos={NEW_LBA}")

    elif ad_type == 1:
        # long_ad: uint32 Extent Length, lb_addr (6 bytes: 4 LBA + 2 partition ref), 2 padding
        ad_len_val = struct.unpack_from('<I', mm, ad_start)[0]
        ad_lba_val = struct.unpack_from('<I', mm, ad_start + 4)[0]
        ad_ptn_val = struct.unpack_from('<H', mm, ad_start + 8)[0]
        print(f"    AD[0] ExtLen       : {ad_len_val & 0x3FFFFFFF:,}")
        print(f"    AD[0] LBA          : {ad_lba_val}")
        print(f"    AD[0] Partition    : {ad_ptn_val}")

        extent_type_bits = (ad_len_val >> 30) << 30
        new_ad_len = extent_type_bits | (NEW_SIZE & 0x3FFFFFFF)
        mm[ad_start     : ad_start + 4] = struct.pack('<I', new_ad_len)
        mm[ad_start + 4 : ad_start + 4] = struct.pack('<I', NEW_LBA)
        print(f"    -> Patched: ExtLen={new_ad_len & 0x3FFFFFFF:,} LBA={NEW_LBA}")
    else:
        print(f"    [!] Unhandled AD type {ad_type}, skipping allocation descriptor patch.")

    # ─────────────────────────────────────────────────────────────────
    # 5. RECOMPUTE UDF Descriptor Checksum (Tag Checksum)
    # ─────────────────────────────────────────────────────────────────
    # UDF Descriptor Tag has a 1-byte Tag Checksum at offset 4.
    # It is computed as: sum of all bytes in the 16-byte tag, excluding byte 4, mod 256.
    tag_bytes = bytearray(mm[fe_base : fe_base + 16])
    tag_bytes[4] = 0  # zero out checksum field before computing
    new_checksum = sum(tag_bytes) & 0xFF
    mm[fe_base + 4] = new_checksum
    print(f"\n[*] Recomputed UDF tag checksum: 0x{new_checksum:02X}")

    # ─────────────────────────────────────────────────────────────────
    # 6. PATCH ISO9660 Directory Record (redundant but correct)
    # ─────────────────────────────────────────────────────────────────
    print("\n[*] Re-verifying ISO9660 directory record patch...")
    search_str = b'\x0ACHAR.ESF;1'
    iso9660_patched = 0
    idx = 0
    while True:
        idx = mm.find(search_str, idx)
        if idx == -1:
            break
        dr_start = idx - 32
        lba_le = struct.unpack_from('<I', mm, dr_start + 2)[0]
        lba_be = struct.unpack_from('>I', mm, dr_start + 6)[0]
        if lba_le == lba_be:
            current_lba = lba_le
            current_size = struct.unpack_from('<I', mm, dr_start + 10)[0]
            print(f"  ISO9660 DR at 0x{dr_start:X}: LBA={current_lba}, Size={current_size:,}")
            if current_lba != NEW_LBA or current_size != NEW_SIZE:
                mm[dr_start + 2 : dr_start + 6]  = struct.pack('<I', NEW_LBA)
                mm[dr_start + 6 : dr_start + 10] = struct.pack('>I', NEW_LBA)
                mm[dr_start + 10: dr_start + 14] = struct.pack('<I', NEW_SIZE)
                mm[dr_start + 14: dr_start + 18] = struct.pack('>I', NEW_SIZE)
                print(f"  -> Updated: LBA={NEW_LBA}, Size={NEW_SIZE:,}")
            else:
                print(f"  -> Already correct, no change needed.")
            iso9660_patched += 1
        idx += len(search_str)

    if iso9660_patched == 0:
        print("  [!] WARNING: No ISO9660 directory record found for CHAR.ESF!")

    mm.close()

# ─────────────────────────────────────────────────────────────────────────
# 7. VERIFY the patch by reading back
# ─────────────────────────────────────────────────────────────────────────
print("\n[*] Verifying patch was applied correctly...")
with open(ISO_PATH, 'rb') as f:
    f.seek(UDF_FE_OFFSET)
    fe_data = f.read(0x200)

verify_info_len = struct.unpack_from('<Q', fe_data, 0x38)[0]
l_ea_v = struct.unpack_from('<I', fe_data, 0xA8)[0]
ad_start_v = 0xB0 + l_ea_v
verify_lba = struct.unpack_from('<I', fe_data, ad_start_v + 4)[0]

print(f"  UDF File Entry Information Length : {verify_info_len:,} bytes {'[OK]' if verify_info_len == NEW_SIZE else '[FAIL - MISMATCH]'}")
print(f"  UDF Allocation Descriptor LBA     : {verify_lba} {'[OK]' if verify_lba == NEW_LBA else '[FAIL - MISMATCH]'}")

print()
if verify_info_len == NEW_SIZE and verify_lba == NEW_LBA:
    print("=" * 70)
    print("  [PASS] DUAL-FILESYSTEM PATCH COMPLETE!")
    print("  The PS2 IOP will now correctly locate and load CHAR.ESF via UDF.")
    print("=" * 70)
else:
    print("=" * 70)
    print("  [FAIL] Verification failed. Review output above.")
    print("=" * 70)
    sys.exit(1)
