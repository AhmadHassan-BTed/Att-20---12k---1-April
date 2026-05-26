#!/usr/bin/env python3
"""
solve_udf_partition_map.py
===========================
The UDF File Entry for CHAR.ESF at sector 337 has:
  - ICB Flag AD type = 7 (custom inline/extended)
  - Allocation descriptor at 0xA8934: 
      bytes 0-3: 1C F6 D7 08 = 148,370,972 (old size)
      bytes 4-7: E4 0C 00 00 = 3300         (partition-relative LBA?)

Hypothesis: UDF Partition starts at a physical sector offset.
  Physical LBA = PartitionStart + LogicalLBA
  If PartitionStart + 3300 = 3578, then PartitionStart = 278.

This script:
1. Reads the UDF Volume Descriptor Sequence to find PartitionStart
2. Calculates what the NEW logical LBA should be for our patched data
3. Patches the allocation descriptor accordingly
4. Handles the case where it's actually just a direct physical LBA
"""
import struct, os, sys

ISO_PATH = 'EQOA_Frontiers_Patched.iso'
NEW_SIZE = os.path.getsize('workspace/FINAL_CHAR_MERGED.ESF')
NEW_PHYSICAL_LBA = 1492368

OLD_SIZE = 148_370_972
OLD_AD_BYTES = bytes.fromhex('1cf6d708e40c0000')

print("=" * 70)
print("  UDF PARTITION MAP + ALLOCATION DESCRIPTOR ANALYSIS")
print("=" * 70)
print()

with open(ISO_PATH, 'rb') as f:
    data = f.read()

# ── Find UDF Volume Recognition Area ──────────────────────────────────────────
print("[*] Searching for UDF Volume Descriptor Sequence...")

# UDF VDS typically starts at sector 256 (0x80000) for DVDs
# Look for Partition Descriptor (tag 0x0108)
partition_start = None
partition_length = None

for sector in range(240, 320):
    off = sector * 2048
    if off + 0x200 > len(data): break
    tag_id = struct.unpack_from('<H', data, off)[0]
    if tag_id == 0x0108:  # Partition Descriptor
        ptn_number = struct.unpack_from('<H', data, off + 0x16)[0]
        ptn_start  = struct.unpack_from('<I', data, off + 0x28)[0]
        ptn_len    = struct.unpack_from('<I', data, off + 0x2C)[0]
        print(f"  Partition Descriptor at sector {sector}:")
        print(f"    Partition Number: {ptn_number}")
        print(f"    Access Start   : {ptn_start}")
        print(f"    Access Length  : {ptn_len}")
        if partition_start is None:
            partition_start = ptn_start
            partition_length = ptn_len

if partition_start is None:
    print("  No Partition Descriptor found in sectors 240-320. Trying wider range...")
    for sector in range(0, 600):
        off = sector * 2048
        if off + 0x200 > len(data): break
        tag_id = struct.unpack_from('<H', data, off)[0]
        if tag_id == 0x0108:
            ptn_start = struct.unpack_from('<I', data, off + 0x28)[0]
            ptn_len   = struct.unpack_from('<I', data, off + 0x2C)[0]
            print(f"  Partition Descriptor at sector {sector}: start={ptn_start}, len={ptn_len}")
            if partition_start is None:
                partition_start = ptn_start
                partition_length = ptn_len

print()

# ── Calculate the logical LBA for the old and new CHAR.ESF ────────────────────
OLD_AD_LOGICAL_LBA = 3300  # bytes 4-7 of the allocation descriptor
OLD_PHYSICAL_LBA   = 3578  # where CHAR.ESF actually was in the ISO

print(f"[*] Allocation Descriptor analysis:")
print(f"    AD bytes: {OLD_AD_BYTES.hex()}")
print(f"    bytes[0:4] LE = {struct.unpack('<I', OLD_AD_BYTES[0:4])[0]:,} (old size)")
print(f"    bytes[4:8] LE = {struct.unpack('<I', OLD_AD_BYTES[4:8])[0]} (logical LBA?)")
print()

if partition_start is not None:
    computed_physical = partition_start + OLD_AD_LOGICAL_LBA
    print(f"    Partition starts at physical sector: {partition_start}")
    print(f"    {partition_start} + {OLD_AD_LOGICAL_LBA} = {computed_physical}")
    if computed_physical == OLD_PHYSICAL_LBA:
        print(f"    *** CONFIRMED: Logical LBA {OLD_AD_LOGICAL_LBA} maps to physical {OLD_PHYSICAL_LBA} ***")
        NEW_LOGICAL_LBA = NEW_PHYSICAL_LBA - partition_start
        print(f"    New logical LBA = {NEW_PHYSICAL_LBA} - {partition_start} = {NEW_LOGICAL_LBA}")
    else:
        print(f"    MISMATCH: expected {OLD_PHYSICAL_LBA}, got {computed_physical}")
        print(f"    Checking if AD bytes are physical LBA directly...")
        if OLD_AD_LOGICAL_LBA == OLD_PHYSICAL_LBA:
            print(f"    *** AD stores physical LBA directly! ***")
            NEW_LOGICAL_LBA = NEW_PHYSICAL_LBA
        else:
            # Try to find what partition_start makes it work
            needed_ptn = OLD_PHYSICAL_LBA - OLD_AD_LOGICAL_LBA
            print(f"    For equation to work, partition_start should be: {needed_ptn}")
            NEW_LOGICAL_LBA = NEW_PHYSICAL_LBA - needed_ptn
else:
    print(f"    No partition descriptor found. Assuming direct physical LBA.")
    if OLD_AD_LOGICAL_LBA == OLD_PHYSICAL_LBA:
        NEW_LOGICAL_LBA = NEW_PHYSICAL_LBA
    else:
        # Use the difference
        diff = OLD_PHYSICAL_LBA - OLD_AD_LOGICAL_LBA
        print(f"    Partition offset = {OLD_PHYSICAL_LBA} - {OLD_AD_LOGICAL_LBA} = {diff}")
        NEW_LOGICAL_LBA = NEW_PHYSICAL_LBA - diff

print()

# ── Build and apply the new allocation descriptor ─────────────────────────────
new_ad_size_le = struct.pack('<I', NEW_SIZE)
new_ad_lba_le  = struct.pack('<I', NEW_LOGICAL_LBA)
new_ad_bytes   = new_ad_size_le + new_ad_lba_le

print(f"[*] Proposed new allocation descriptor:")
print(f"    Old: {OLD_AD_BYTES.hex()}")
print(f"    New: {new_ad_bytes.hex()}")
print(f"    -> Size: {OLD_SIZE:,} -> {NEW_SIZE:,}")
print(f"    -> LBA:  {OLD_AD_LOGICAL_LBA} -> {NEW_LOGICAL_LBA}")
print()

# ── Confirm the UDF File Entry location ───────────────────────────────────────
UDF_FE_OFF    = 337 * 2048  # 0xA8800
UDF_FE_AD_OFF = UDF_FE_OFF + 0x134  # Offset 0x134 within sector = where old size was found

print(f"[*] UDF File Entry at offset 0x{UDF_FE_OFF:X}")
print(f"    Allocation descriptor at offset 0x{UDF_FE_AD_OFF:X} (0x134 within FE sector)")
print()

# Verify we're looking at the right bytes
current_bytes = data[UDF_FE_AD_OFF:UDF_FE_AD_OFF+8]
print(f"[*] Current bytes at 0x{UDF_FE_AD_OFF:X}: {current_bytes.hex()}")
print(f"    Expected old AD:                   {OLD_AD_BYTES.hex()}")

if current_bytes == OLD_AD_BYTES:
    print(f"    *** MATCH - this is the allocation descriptor to patch ***")
elif current_bytes[:4] == OLD_AD_BYTES[:4]:
    print(f"    Size matches but LBA part differs:")
    print(f"    Current LBA part: {struct.unpack('<I', current_bytes[4:8])[0]}")
    print(f"    Note: patch already applied the size at 0x0038, 0x0134 may still be correct")
else:
    print(f"    No match at 0x134. Let's find the AD bytes in the sector...")
    sector_data = data[UDF_FE_OFF:UDF_FE_OFF+2048]
    pos = sector_data.find(OLD_AD_BYTES)
    if pos != -1:
        print(f"    Found old AD at sector offset 0x{pos:04X} (absolute 0x{UDF_FE_OFF+pos:X})")
        UDF_FE_AD_OFF = UDF_FE_OFF + pos
    else:
        # Try just searching for the old LBA part with size already patched
        print(f"    Old AD not found (may have been partially patched). Looking for LBA {OLD_AD_LOGICAL_LBA}...")
        lba_bytes = struct.pack('<I', OLD_AD_LOGICAL_LBA)
        pos = sector_data.find(lba_bytes)
        if pos != -1:
            print(f"    Found LBA at sector offset 0x{pos:04X}")
            UDF_FE_AD_OFF = UDF_FE_OFF + pos - 4  # back 4 for size

print()
print(f"[*] Will patch at absolute offset 0x{UDF_FE_AD_OFF:X}")
print()

# ── Write the patch ────────────────────────────────────────────────────────────
user_confirm = input("Apply this patch to the ISO? (yes/no): ").strip().lower()
if user_confirm != 'yes':
    print("Aborted.")
    sys.exit(0)

print(f"\n[*] Patching ISO at offset 0x{UDF_FE_AD_OFF:X}...")
try:
    with open(ISO_PATH, 'r+b') as f:
        f.seek(UDF_FE_AD_OFF)
        f.write(new_ad_bytes)
    print("[+] Written successfully!")
except PermissionError as e:
    print(f"[-] Permission error: {e}")
    print("    Is PCSX2 or another program using the ISO? Close it first.")
    sys.exit(1)

# Also patch the Information Length at offset 0x38 of the File Entry
print(f"[*] Also patching UDF FE Information Length at 0x{UDF_FE_OFF+0x38:X}...")
try:
    with open(ISO_PATH, 'r+b') as f:
        f.seek(UDF_FE_OFF + 0x38)
        f.write(struct.pack('<Q', NEW_SIZE))
    print("[+] Information Length updated!")
except PermissionError as e:
    print(f"[-] Permission error: {e}")

# Verify
with open(ISO_PATH, 'rb') as f:
    f.seek(UDF_FE_AD_OFF)
    verify = f.read(8)
print(f"\n[*] Verify: bytes at 0x{UDF_FE_AD_OFF:X}: {verify.hex()}")
print(f"    Expected:                          {new_ad_bytes.hex()}")
if verify == new_ad_bytes:
    print("\n  [PASS] UDF Allocation Descriptor patched successfully!")
else:
    print("\n  [FAIL] Patch did not take effect!")
