#!/usr/bin/env python3
"""
test_vanilla_frontiers.py
=========================
THE MOST FUNDAMENTAL QUESTION:
Does the UNMODIFIED Frontiers ISO show characters at all, or is the
"invisible character" issue unrelated to our patches?

This script checks:
1. Are both ISOs (Frontiers and Patched) byte-identical EXCEPT for CHAR.ESF?
2. Is the old CHAR.ESF data still present at LBA 3578 in the patched ISO?
3. What does the UDF ACTUALLY point to now?
4. If we could just use the UNMODIFIED Frontiers CHAR.ESF, would it work?
5. Compare the first model (0x05AEBA67) payload structure between original and grafted

This will tell us if the problem is:
A) Our grafting made bad payloads (geometry doesn't match header expectations)
B) The game isn't loading CHAR.ESF at all for some other reason
C) The characters are invisible for a reason unrelated to CHAR.ESF
"""
import struct, os

# CRITICAL TEST: Are the two ISOs identical except CHAR.ESF?
# Read sector-by-sector and compare

FRONTIERS = 'EQOA_Frontiers.iso'
PATCHED   = 'EQOA_Frontiers_Patched.iso'

print("=" * 70)
print("  CRITICAL: Are characters visible in the UNMODIFIED Frontiers ISO?")
print("=" * 70)
print()

# Compare first 3578 * 2048 bytes (everything before CHAR.ESF)
print("[1] Comparing ISO data BEFORE CHAR.ESF (sectors 0-3577)...")
SECTOR_SIZE = 2048
CHAR_ESF_LBA = 3578

with open(FRONTIERS, 'rb') as ff, open(PATCHED, 'rb') as pf:
    diffs_before = 0
    for sector in range(CHAR_ESF_LBA):
        f_data = ff.read(SECTOR_SIZE)
        p_data = pf.read(SECTOR_SIZE)
        if f_data != p_data:
            diffs_before += 1
            if diffs_before <= 5:
                # Find which bytes differ
                for i in range(SECTOR_SIZE):
                    if i < len(f_data) and i < len(p_data) and f_data[i] != p_data[i]:
                        print(f"  Sector {sector}, byte {i}: 0x{f_data[i]:02X} -> 0x{p_data[i]:02X}")
                        break

print(f"  Sectors with differences before CHAR.ESF: {diffs_before}")

if diffs_before == 0:
    print("  >>> PERFECT MATCH: Everything before CHAR.ESF is identical <<<")
elif diffs_before <= 3:
    print("  >>> Only minor differences (likely directory record updates) <<<")
else:
    print(f"  >>> {diffs_before} sectors differ — something unexpected was changed! <<<")

print()

# Check if old CHAR.ESF data is still at LBA 3578 in patched ISO
print("[2] Checking old CHAR.ESF data at LBA 3578 in PATCHED ISO...")
with open(PATCHED, 'rb') as f:
    f.seek(CHAR_ESF_LBA * SECTOR_SIZE)
    old_data = f.read(64)
print(f"  First 16 bytes at LBA 3578: {old_data[:16].hex()}")
print(f"  Magic: {old_data[:4]}")
if old_data[:4] == b'FJBO':
    print("  >>> OLD CHAR.ESF DATA IS STILL PRESENT at LBA 3578! <<<")
    print("  >>> If UDF still points here, game reads the old, unmodified data! <<<")

print()

# Read where UDF points
print("[3] Where does UDF currently point for CHAR.ESF?")
with open(PATCHED, 'rb') as f:
    f.seek(0xA8934)
    ad = f.read(8)
    ad_size = struct.unpack('<I', ad[0:4])[0]
    ad_lba  = struct.unpack('<I', ad[4:8])[0]
    phys_lba = ad_lba + 278  # partition offset
    
    print(f"  UDF Allocation: size={ad_size:,}, logical_lba={ad_lba}, physical_lba={phys_lba}")
    
    # Read data at the UDF-pointed location
    f.seek(phys_lba * SECTOR_SIZE)
    udf_data = f.read(64)
    print(f"  Data at physical LBA {phys_lba}: {udf_data[:16].hex()}")
    if udf_data[:4] == b'FJBO':
        print("  >>> UDF points to valid FJBO data <<<")

print()

# THE KEY TEST: Does a grafted payload have valid internal structure?
print("[4] Comparing one grafted payload structure vs original Frontiers payload...")
# Pick asset 0x05AEBA67 (the first target)
import json
with open('workspace/target_assets.json') as f:
    targets = json.load(f)

target = targets[1]  # 0x05AEBA67
exp_hash = int(target['expansion_hash'], 16)
exp_offset = target['expansion_offset']
exp_length = target['expansion_length']
bin_path = f"workspace/payloads/asset_{target['expansion_hash']}.bin"

# Read original Frontiers payload at expansion_offset inside CHAR.ESF
with open(FRONTIERS, 'rb') as f:
    f.seek(CHAR_ESF_LBA * SECTOR_SIZE + exp_offset)
    orig_payload = f.read(exp_length)

# Read grafted payload
with open(bin_path, 'rb') as f:
    graft_payload = f.read()

print(f"  Asset: {target['expansion_hash']}")
print(f"  Original Frontiers payload: {len(orig_payload):,} bytes")
print(f"  Grafted payload:            {len(graft_payload):,} bytes")
print()

# Parse first few nodes of each
def show_node_tree(data, label, depth=0, max_nodes=30):
    pos = 0
    nodes_shown = 0
    stack = [(0, depth)]  # (position, depth)
    
    print(f"  {label} node tree:")
    while stack and nodes_shown < max_nodes:
        pos, d = stack.pop(0)
        if pos + 12 > len(data):
            break
        type_id = struct.unpack_from('<I', data, pos)[0]
        data_size = struct.unpack_from('<I', data, pos+4)[0]
        child_count = struct.unpack_from('<I', data, pos+8)[0]
        
        indent = "    " * d
        print(f"  {indent}[0x{pos:06X}] type=0x{type_id:05X}, size={data_size:,}, children={child_count}")
        nodes_shown += 1
        
        next_pos = pos + 12
        if child_count > 0:
            for i in range(min(child_count, 10)):
                if next_pos + 12 <= len(data):
                    stack.insert(i, (next_pos, d+1))
                    # Skip to next sibling
                    c_type = struct.unpack_from('<I', data, next_pos)[0]
                    c_size = struct.unpack_from('<I', data, next_pos+4)[0]
                    next_pos += 12 + c_size

show_node_tree(orig_payload, "ORIGINAL Frontiers", max_nodes=25)
print()
show_node_tree(graft_payload, "GRAFTED payload", max_nodes=25)
print()

# Check: Do the root types and child counts match?
orig_root_type = struct.unpack_from('<I', orig_payload, 0)[0]
graft_root_type = struct.unpack_from('<I', graft_payload, 0)[0]
orig_root_children = struct.unpack_from('<I', orig_payload, 8)[0]
graft_root_children = struct.unpack_from('<I', graft_payload, 8)[0]

print(f"  Root type:     orig=0x{orig_root_type:05X}, grafted=0x{graft_root_type:05X} -> {'MATCH' if orig_root_type == graft_root_type else 'MISMATCH!'}")
print(f"  Root children: orig={orig_root_children}, grafted={graft_root_children} -> {'MATCH' if orig_root_children == graft_root_children else 'MISMATCH!'}")

# Compare child type sequence
print()
print("[5] Comparing child type sequences (first 20):")
def get_child_types(data, count):
    types = []
    pos = 12  # skip root header
    for _ in range(count):
        if pos + 12 > len(data):
            break
        t = struct.unpack_from('<I', data, pos)[0]
        s = struct.unpack_from('<I', data, pos+4)[0]
        types.append(t)
        pos += 12 + s
    return types

orig_types = get_child_types(orig_payload, orig_root_children)
graft_types = get_child_types(graft_payload, graft_root_children)

for i in range(max(len(orig_types), len(graft_types))):
    ot = f"0x{orig_types[i]:05X}" if i < len(orig_types) else "---"
    gt = f"0x{graft_types[i]:05X}" if i < len(graft_types) else "---"
    match = "OK" if i < len(orig_types) and i < len(graft_types) and orig_types[i] == graft_types[i] else "DIFF!"
    print(f"  [{i:2d}] orig={ot}  grafted={gt}  {match}")
