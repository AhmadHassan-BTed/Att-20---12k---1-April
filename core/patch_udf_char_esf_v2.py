#!/usr/bin/env python3
"""
patch_udf_char_esf_v2.py
========================
Surgically patches the UDF File Entry for CHAR.ESF at sector 337.

KEY DISCOVERY: The UDF File Entry uses ICB flags AD type = 7, which means
the allocation information is stored as "Data" directly in the ICB itself
(not as short_ad, long_ad, or ext_ad pointers). This is called "Inline Data"
or the "ICBTag.AllocType = Extended AD" mode used differently.

Actually: AD type 7 in ECMA-167 §14.8.1-14.8.4 is ILLEGAL/RESERVED.
This is almost certainly a CUSTOM or OBFUSCATED format. The game engine's
IOP module reads the UDF in a custom way, not via standard UDF parsing.

STRATEGY: Instead of trying to parse the custom AD format, we:
1. Hex-dump the entire File Entry from sector 337 to understand the format
2. Search for the old LBA (3578 = 0x00000DFA) as a 4-byte value within it  
3. Search for the old size (148,370,972 = 0x08D7E81C) as a 4 or 8-byte value
4. Patch those bytes directly

This is low-level binary surgery - exactly what needs to happen.
"""
import struct, os, sys

ISO_PATH  = 'output/EQOA_Frontiers_Patched.iso'
ESF_PATH  = 'workspace/FINAL_CHAR_MERGED.ESF'
PARTITION_OFFSET = 278
NEW_SIZE  = os.path.getsize(ESF_PATH)
NEW_PHYS_LBA = 1492368
NEW_LBA   = NEW_PHYS_LBA - PARTITION_OFFSET

# The OLD values we're looking for to locate and replace
OLD_SIZE  = 148_370_972   # 0x08D7E81C
OLD_PHYS_LBA = 3578
OLD_LBA   = OLD_PHYS_LBA - PARTITION_OFFSET

UDF_FE_SECTOR = 337
UDF_FE_OFF    = UDF_FE_SECTOR * 2048  # 0xA8800

print("=" * 70)
print("  UDF FILE ENTRY BINARY SURGERY FOR CHAR.ESF")
print("=" * 70)
print(f"  OLD Size : {OLD_SIZE:,}  (0x{OLD_SIZE:08X})")
print(f"  NEW Size : {NEW_SIZE:,}  (0x{NEW_SIZE:08X})")
print(f"  OLD LBA  : {OLD_LBA}  (0x{OLD_LBA:08X})")
print(f"  NEW LBA  : {NEW_LBA}  (0x{NEW_LBA:08X})")
print()

with open(ISO_PATH, 'rb') as f:
    f.seek(UDF_FE_OFF)
    fe_raw = bytearray(f.read(2048))

# Check if already patched
curr_size = struct.unpack_from('<Q', fe_raw, 0x38)[0]
l_ea_curr = struct.unpack_from('<I', fe_raw, 0xA8)[0]
ad_start_curr = 0xB0 + l_ea_curr
curr_lba = struct.unpack_from('<I', fe_raw, ad_start_curr + 4)[0]

if curr_size == NEW_SIZE and curr_lba == NEW_LBA:
    print("[+] UDF File Entry is ALREADY successfully patched to:")
    print(f"    Size: {curr_size:,} bytes")
    print(f"    LBA  : {curr_lba}")
    print("=" * 70)
    print("  [PASS] UDF File Entry patched successfully!")
    print("         The PS2 IOP will now read CHAR.ESF from the correct location.")
    print("=" * 70)
    sys.exit(0)

print("[*] UDF File Entry hex dump (first 256 bytes):")
for row in range(0, 256, 16):
    hex_part = ' '.join(f'{b:02X}' for b in fe_raw[row:row+16])
    asc_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in fe_raw[row:row+16])
    print(f"  {row:04X}: {hex_part:<48}  {asc_part}")
print()

# ── Search for OLD_SIZE as various encodings ──────────────────────────────────
print(f"[*] Searching for OLD_SIZE = 0x{OLD_SIZE:08X} ({OLD_SIZE:,})")
patches_applied = []

# As 4-byte LE
old_sz_4le = struct.pack('<I', OLD_SIZE)
pos = 0
while True:
    pos = fe_raw.find(old_sz_4le, pos)
    if pos == -1: break
    print(f"  Found OLD_SIZE (4-byte LE) at offset 0x{pos:04X} within FE")
    patches_applied.append(('size_4le', pos))
    pos += 1

# As 8-byte LE (this is the official field at offset 0x38)
old_sz_8le = struct.pack('<Q', OLD_SIZE)
pos = 0
while True:
    pos = fe_raw.find(old_sz_8le, pos)
    if pos == -1: break
    print(f"  Found OLD_SIZE (8-byte LE) at offset 0x{pos:04X} within FE")
    patches_applied.append(('size_8le', pos))
    pos += 1

# As 4-byte BE
old_sz_4be = struct.pack('>I', OLD_SIZE)
pos = 0
while True:
    pos = fe_raw.find(old_sz_4be, pos)
    if pos == -1: break
    print(f"  Found OLD_SIZE (4-byte BE) at offset 0x{pos:04X} within FE")
    patches_applied.append(('size_4be', pos))
    pos += 1

print()
print(f"[*] Searching for OLD_LBA = 0x{OLD_LBA:08X} ({OLD_LBA})")

# As 4-byte LE
old_lba_4le = struct.pack('<I', OLD_LBA)
pos = 0
while True:
    pos = fe_raw.find(old_lba_4le, pos)
    if pos == -1: break
    print(f"  Found OLD_LBA (4-byte LE) at offset 0x{pos:04X} within FE")
    patches_applied.append(('lba_4le', pos))
    pos += 1

# As 4-byte BE
old_lba_4be = struct.pack('>I', OLD_LBA)
pos = 0
while True:
    pos = fe_raw.find(old_lba_4be, pos)
    if pos == -1: break
    print(f"  Found OLD_LBA (4-byte BE) at offset 0x{pos:04X} within FE")
    patches_applied.append(('lba_4be', pos))
    pos += 1

# Also check logical block count (72447 = 0x11AFF)
old_blocks = 72447
new_blocks  = (NEW_SIZE + 2047) // 2048
old_blk_8le = struct.pack('<Q', old_blocks)
pos = 0
while True:
    pos = fe_raw.find(old_blk_8le, pos)
    if pos == -1: break
    print(f"  Found OLD_BLOCKS={old_blocks} (8-byte LE) at offset 0x{pos:04X}")
    patches_applied.append(('blocks_8le', pos))
    pos += 1

print()

if not patches_applied:
    print("[!] Nothing found using standard binary search.")
    print("    Trying to find any 4-byte value close to 3578 in the FE...")
    for i in range(0, min(512, len(fe_raw)) - 3):
        v = struct.unpack_from('<I', fe_raw, i)[0]
        if 3500 <= v <= 3700:
            print(f"  Offset 0x{i:04X}: 4-byte LE = {v} (close to OLD_LBA=3578)")
    print()
    print("    Trying to find any 4-byte value close to 148370972 in the FE...")
    for i in range(0, min(512, len(fe_raw)) - 3):
        v = struct.unpack_from('<I', fe_raw, i)[0]
        if 148_000_000 <= v <= 149_000_000:
            print(f"  Offset 0x{i:04X}: 4-byte LE = {v:,}")
    sys.exit(1)

# ── Apply patches ─────────────────────────────────────────────────────────────
print("[*] Applying binary patches to UDF File Entry...")
for ptype, poff in patches_applied:
    if ptype == 'size_8le':
        fe_raw[poff:poff+8] = struct.pack('<Q', NEW_SIZE)
        print(f"  Patched size (8-byte LE) at 0x{poff:04X}: {OLD_SIZE:,} -> {NEW_SIZE:,}")
    elif ptype == 'size_4le':
        fe_raw[poff:poff+4] = struct.pack('<I', NEW_SIZE)
        print(f"  Patched size (4-byte LE) at 0x{poff:04X}: {OLD_SIZE} -> {NEW_SIZE}")
    elif ptype == 'size_4be':
        fe_raw[poff:poff+4] = struct.pack('>I', NEW_SIZE)
        print(f"  Patched size (4-byte BE) at 0x{poff:04X}: {OLD_SIZE} -> {NEW_SIZE}")
    elif ptype == 'lba_4le':
        fe_raw[poff:poff+4] = struct.pack('<I', NEW_LBA)
        print(f"  Patched LBA (4-byte LE) at 0x{poff:04X}: {OLD_LBA} -> {NEW_LBA}")
    elif ptype == 'lba_4be':
        fe_raw[poff:poff+4] = struct.pack('>I', NEW_LBA)
        print(f"  Patched LBA (4-byte BE) at 0x{poff:04X}: {OLD_LBA} -> {NEW_LBA}")
    elif ptype == 'blocks_8le':
        fe_raw[poff:poff+8] = struct.pack('<Q', new_blocks)
        print(f"  Patched blocks (8-byte LE) at 0x{poff:04X}: {old_blocks} -> {new_blocks}")

# ── Recompute UDF Tag Checksum ─────────────────────────────────────────────────
tag_bytes = bytearray(fe_raw[:16])
tag_bytes[4] = 0
new_cksum = sum(tag_bytes) & 0xFF
fe_raw[4] = new_cksum
print(f"\n[*] Recomputed UDF tag checksum: 0x{new_cksum:02X}")

# ── Write back ────────────────────────────────────────────────────────────────
print(f"\n[*] Writing patched File Entry back to ISO at offset 0x{UDF_FE_OFF:X}...")
with open(ISO_PATH, 'r+b') as f:
    f.seek(UDF_FE_OFF)
    f.write(bytes(fe_raw))

# ── Also patch other File Entries if they also reference CHAR.ESF ─────────────
# Check sector 450 and 451 which appeared in the scan
print("\n[*] Checking additional large File Entries (sectors 450, 451)...")
for extra_sector in [450, 451]:
    extra_off = extra_sector * 2048
    with open(ISO_PATH, 'rb') as f:
        f.seek(extra_off)
        extra_raw = bytearray(f.read(2048))
    tag_id = struct.unpack_from('<H', extra_raw, 0)[0]
    info_len = struct.unpack_from('<Q', extra_raw, 0x38)[0]
    print(f"  Sector {extra_sector}: tag=0x{tag_id:04X}, size={info_len:,}")
    # These are different files (>900MB, 192MB), not CHAR.ESF

print("\n[*] Verifying fix...")
with open(ISO_PATH, 'rb') as f:
    f.seek(UDF_FE_OFF)
    verify = f.read(512)

new_info_len = struct.unpack_from('<Q', verify, 0x38)[0]
l_ea_val = struct.unpack_from('<I', verify, 0xA8)[0]
ad_start_val = 0xB0 + l_ea_val
new_lba_val = struct.unpack_from('<I', verify, ad_start_val + 4)[0]

print(f"  UDF File Entry Information Length: {new_info_len:,} (expected {NEW_SIZE:,})")
print(f"  UDF Allocation Descriptor LBA    : {new_lba_val} (expected {NEW_LBA})")

if new_info_len == NEW_SIZE and new_lba_val == NEW_LBA:
    print()
    print("=" * 70)
    print("  [PASS] UDF File Entry patched successfully!")
    print("         The PS2 IOP will now read CHAR.ESF from the correct location.")
    print("=" * 70)
else:
    print()
    print(f"  [FAIL] Verification mismatch: size={new_info_len} (exp {NEW_SIZE}), LBA={new_lba_val} (exp {NEW_LBA})")
    sys.exit(1)
