#!/usr/bin/env python3
"""
definitive_comparison.py
========================
DEFINITIVE test: byte-for-byte compare the ORIGINAL Frontiers CHAR.ESF
against our FINAL_CHAR_MERGED.ESF.

If the original Frontiers game shows characters fine, then the difference
between these two files IS the problem.

We need to understand:
1. What bytes actually changed?
2. Are the changes only in the 11 model payloads (expected)?
3. Or did we corrupt the surrounding structure?
"""
import struct, os, sys

FRONTIERS_ISO = 'EQOA_Frontiers.iso'
MERGED_ESF    = 'workspace/FINAL_CHAR_MERGED.ESF'

# ─── Read both ESFs ──────────────────────────────────────────────────────────
print("[*] Reading original Frontiers CHAR.ESF from ISO...")
with open(FRONTIERS_ISO, 'rb') as f:
    dir_data = f.read(1024 * 1024)

idx = dir_data.find(b'CHAR.ESF;1')
dr_start = idx - 33
front_lba  = struct.unpack_from('<I', dir_data, dr_start + 2)[0]
front_size = struct.unpack_from('<I', dir_data, dr_start + 10)[0]

with open(FRONTIERS_ISO, 'rb') as f:
    f.seek(front_lba * 2048)
    original = f.read(front_size)

print(f"  Original: {len(original):,} bytes from LBA {front_lba}")

with open(MERGED_ESF, 'rb') as f:
    merged = f.read()

print(f"  Merged:   {len(merged):,} bytes")
print(f"  Size delta: {len(merged) - len(original):+,} bytes")
print()

# ─── Compare byte-by-byte ────────────────────────────────────────────────────
print("[*] Finding all differences...")

min_len = min(len(original), len(merged))
diff_regions = []
in_diff = False
diff_start = 0

for i in range(min_len):
    if original[i] != merged[i]:
        if not in_diff:
            diff_start = i
            in_diff = True
    else:
        if in_diff:
            diff_regions.append((diff_start, i - diff_start))
            in_diff = False

if in_diff:
    diff_regions.append((diff_start, min_len - diff_start))

# If merged is longer, count the tail
if len(merged) > len(original):
    diff_regions.append((len(original), len(merged) - len(original)))

print(f"  Total different regions: {len(diff_regions)}")
print(f"  Total different bytes:   {sum(s for _,s in diff_regions):,}")
print()

# Show the first few diff regions
print("[*] Diff regions (showing first 20):")
for i, (offset, size) in enumerate(diff_regions[:20]):
    # Show context from original
    if offset < len(original):
        orig_preview = original[offset:offset+min(16, size)].hex()
    else:
        orig_preview = "(beyond original EOF)"
    
    if offset < len(merged):
        merg_preview = merged[offset:offset+min(16, size)].hex()
    else:
        merg_preview = "(beyond merged EOF)"
    
    print(f"  [{i:2d}] Offset: 0x{offset:08X} ({offset:,}), Size: {size:,} bytes")
    print(f"       Orig: {orig_preview}")
    print(f"       Merg: {merg_preview}")

print()

# ─── Check if changes correspond to our 11 target model locations ───────────
print("[*] Mapping differences to target assets...")

import json
with open('workspace/target_assets.json', 'r') as f:
    targets = json.load(f)

# The ESF has a 16-byte FJBO header, then a 12-byte root node header
# Then a 12-byte model container header, then model children
# Each model starts at expansion_offset (relative to CHAR.ESF data start? or file start?)

for t in targets:
    exp_offset = t['expansion_offset']
    exp_length = t['expansion_length']
    exp_hash   = t['expansion_hash']
    
    # Check if any diff region overlaps with this asset's range
    overlapping = []
    for d_off, d_size in diff_regions:
        d_end = d_off + d_size
        a_end = exp_offset + exp_length
        if d_off < a_end and d_end > exp_offset:
            overlap_start = max(d_off, exp_offset)
            overlap_end   = min(d_end, a_end)
            overlapping.append((overlap_start - exp_offset, overlap_end - overlap_start))
    
    if overlapping:
        total_overlap = sum(s for _,s in overlapping)
        print(f"  {exp_hash}: {len(overlapping)} diff regions, {total_overlap:,} bytes changed (out of {exp_length:,})")
    else:
        print(f"  {exp_hash}: NO CHANGES DETECTED in expansion range [{exp_offset}:{exp_offset+exp_length}]")

print()

# ─── Most important: check if ANY structure OUTSIDE the 11 models changed ──
print("[*] Checking for changes OUTSIDE the 11 target assets...")
target_ranges = [(t['expansion_offset'], t['expansion_offset'] + t['expansion_length']) for t in targets]
outside_diffs = []

for d_off, d_size in diff_regions:
    d_end = d_off + d_size
    # Check if this diff is entirely outside all target ranges
    inside_any = False
    for t_start, t_end in target_ranges:
        if d_off >= t_start and d_end <= t_end:
            inside_any = True
            break
    if not inside_any:
        outside_diffs.append((d_off, d_size))

print(f"  Diff regions outside target assets: {len(outside_diffs)}")
for d_off, d_size in outside_diffs[:10]:
    orig_bytes = original[d_off:d_off+min(32, d_size)].hex() if d_off < len(original) else "(EOF)"
    merg_bytes = merged[d_off:d_off+min(32, d_size)].hex() if d_off < len(merged) else "(EOF)"
    print(f"    Offset 0x{d_off:08X} ({d_off:,}), Size: {d_size:,}")
    print(f"      Orig: {orig_bytes}")
    print(f"      Merg: {merg_bytes}")

if not outside_diffs:
    print("  ALL changes are within target asset ranges — structural integrity OK")
else:
    print(f"\n  !!! {len(outside_diffs)} CHANGES OUTSIDE TARGET ASSETS !!!")
    print("  This indicates structural corruption or shifted offsets!")
