#!/usr/bin/env python3
"""
ground_truth_audit.py
=====================
STOP looking at filesystem. START looking at what the game engine actually needs.

This script answers THE question: when the Frontiers game engine reads CHAR.ESF
and tries to render a character, what EXACTLY is it looking for, and does our
merged ESF have it in the right format?

Step 1: Read the UNMODIFIED Frontiers CHAR.ESF from the original ISO
Step 2: Read our FINAL_CHAR_MERGED.ESF
Step 3: Compare the top-level structure (how many models, their hashes, types)
Step 4: For each of the 11 target assets, compare the node tree structure
        between what Frontiers originally had vs what we injected
Step 5: Check if the game might be loading characters from a DIFFERENT file

The goal: find the structural mismatch that causes the engine to skip rendering.
"""

import struct, os, sys, json

# ─── FJBO Node Tree Parser ──────────────────────────────────────────────────
def parse_fjbo_header(data, offset=0):
    """Parse the FJBO file header."""
    magic = data[offset:offset+4]
    if magic != b'FJBO':
        return None, offset
    version = struct.unpack_from('<I', data, offset+4)[0]
    return {'magic': magic, 'version': version}, offset + 16  # 16-byte file header

def parse_node(data, pos, depth=0, max_depth=3):
    """Parse a node from the FJBO tree. Limited depth to avoid huge recursion."""
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos+4)[0]
    child_count = struct.unpack_from('<I', data, pos+8)[0]
    
    node = {
        'offset': pos,
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'children': []
    }
    
    next_pos = pos + 12
    if child_count == 0:
        next_pos += data_size
    else:
        if depth < max_depth:
            for _ in range(child_count):
                if next_pos + 12 > len(data):
                    break
                child, next_pos = parse_node(data, next_pos, depth+1, max_depth)
                if child:
                    node['children'].append(child)
        else:
            next_pos += data_size
    
    return node, next_pos

def find_hash_in_node(data, node):
    """Look for the 0x22000 hash node inside a model subtree."""
    if node['child_count'] == 0:
        return None
    for child in node['children']:
        if child['type_id'] == 0x22000:
            # Hash is typically 4 bytes at the start of inline data
            hash_offset = child['offset'] + 12
            if hash_offset + 4 <= len(data):
                return struct.unpack_from('<I', data, hash_offset)[0]
    # Also check the node itself
    if node['type_id'] == 0x22000:
        hash_offset = node['offset'] + 12
        if hash_offset + 4 <= len(data):
            return struct.unpack_from('<I', data, hash_offset)[0]
    return None


# ─── Main ────────────────────────────────────────────────────────────────────
# Check for unmodified Frontiers CHAR.ESF
FRONTIERS_ISO = 'EQOA_Frontiers.iso'
MERGED_ESF    = 'workspace/FINAL_CHAR_MERGED.ESF'
ORIGINAL_ISO  = 'EQOA_Original.iso'

print("=" * 70)
print("  GROUND TRUTH AUDIT: What does the Frontiers engine ACTUALLY need?")
print("=" * 70)
print()

# ─── Step 0: Find CHAR.ESF LBA in unmodified Frontiers ISO ──────────────────
print("[Step 0] Locating CHAR.ESF in the unmodified Frontiers ISO...")
with open(FRONTIERS_ISO, 'rb') as f:
    # Read the first 1MB to find the directory record
    dir_data = f.read(1024 * 1024)

search = b'CHAR.ESF;1'
idx = dir_data.find(search)
if idx == -1:
    print("  CHAR.ESF not found in directory!")
    sys.exit(1)

dr_start = idx - 33
frontiers_lba  = struct.unpack_from('<I', dir_data, dr_start + 2)[0]
frontiers_size = struct.unpack_from('<I', dir_data, dr_start + 10)[0]
print(f"  Frontiers CHAR.ESF: LBA={frontiers_lba}, Size={frontiers_size:,}")

# ─── Step 1: Read first N bytes of both ESFs to compare structure ────────────
HEADER_READ = 1024 * 1024  # Read first 1MB of each for header comparison

print(f"\n[Step 1] Reading ESF headers for structural comparison...")

with open(FRONTIERS_ISO, 'rb') as f:
    f.seek(frontiers_lba * 2048)
    frontiers_esf_head = f.read(min(HEADER_READ, frontiers_size))

with open(MERGED_ESF, 'rb') as f:
    merged_esf_head = f.read(HEADER_READ)

print(f"  Frontiers ESF header: {frontiers_esf_head[:16].hex()}")
print(f"  Merged ESF header   : {merged_esf_head[:16].hex()}")

# Parse the FJBO header
fh1, fpos1 = parse_fjbo_header(frontiers_esf_head)
fh2, fpos2 = parse_fjbo_header(merged_esf_head)

print(f"  Frontiers FJBO version: {fh1['version'] if fh1 else 'NOT FJBO!'}")
print(f"  Merged FJBO version   : {fh2['version'] if fh2 else 'NOT FJBO!'}")

# ─── Step 2: Parse root node and model container ────────────────────────────
print(f"\n[Step 2] Parsing root nodes...")

root1, _ = parse_node(frontiers_esf_head, fpos1, max_depth=2)
root2, _ = parse_node(merged_esf_head, fpos2, max_depth=2)

if root1:
    print(f"  Frontiers root: type=0x{root1['type_id']:05X}, size={root1['data_size']:,}, children={root1['child_count']}")
    for c in root1['children']:
        print(f"    child: type=0x{c['type_id']:05X}, size={c['data_size']:,}, children={c['child_count']}")

if root2:
    print(f"  Merged root: type=0x{root2['type_id']:05X}, size={root2['data_size']:,}, children={root2['child_count']}")
    for c in root2['children']:
        print(f"    child: type=0x{c['type_id']:05X}, size={c['data_size']:,}, children={c['child_count']}")

print()

# ─── Step 3: Compare model counts ──────────────────────────────────────────
print(f"[Step 3] Comparing model containers...")

# Find the 0x0A010 model container in both
mc1 = None
mc2 = None
for c in (root1['children'] if root1 else []):
    if c['type_id'] == 0x0A010:
        mc1 = c
for c in (root2['children'] if root2 else []):
    if c['type_id'] == 0x0A010:
        mc2 = c

if mc1:
    print(f"  Frontiers model container: {mc1['child_count']} models, size={mc1['data_size']:,}")
if mc2:
    print(f"  Merged model container  : {mc2['child_count']} models, size={mc2['data_size']:,}")
    
if mc1 and mc2:
    delta_models = mc2['child_count'] - mc1['child_count']
    delta_size   = mc2['data_size'] - mc1['data_size']
    print(f"  Delta: {delta_models:+d} models, {delta_size:+,} bytes")
    if delta_models > 0:
        print(f"  *** MERGED has MORE models than original Frontiers ***")
        print(f"  *** This means we APPENDED models rather than replacing them ***")

print()

# ─── Step 4: Inspect individual target payloads ─────────────────────────────
print(f"[Step 4] Inspecting the 11 injected payloads...")

with open('workspace/target_assets.json', 'r') as f:
    targets = json.load(f)

for t in targets:
    hash_str = t['expansion_hash']
    orig_type = t['original_type']
    exp_type  = t['expansion_type']
    exp_size  = t['expansion_length']
    
    bin_path = f"workspace/payloads/asset_{hash_str}.bin"
    if os.path.exists(bin_path):
        with open(bin_path, 'rb') as f:
            payload = f.read(64)
        
        p_type = struct.unpack_from('<I', payload, 0)[0]
        p_size = struct.unpack_from('<I', payload, 4)[0]
        p_children = struct.unpack_from('<I', payload, 8)[0]
        
        bin_file_size = os.path.getsize(bin_path)
        
        type_ok = (p_type == int(exp_type, 16))
        size_expected = p_size == exp_size or abs(p_size - (bin_file_size - 12)) < 16
        
        status = ""
        if p_type == int(orig_type, 16):
            status = "!!! STILL ORIGINAL TYPE - NOT UPGRADED !!!"
        elif not type_ok:
            status = f"!!! UNEXPECTED TYPE 0x{p_type:05X} !!!"
        else:
            status = "type OK"
        
        print(f"  {hash_str}: type=0x{p_type:05X} ({status}), children={p_children}, "
              f"data_size={p_size:,}, file={bin_file_size:,}")
    else:
        print(f"  {hash_str}: PAYLOAD FILE MISSING!")

print()

# ─── Step 5: Check what OTHER files reference character models ──────────────
print(f"[Step 5] Checking CHARCUST.CSF — what does it tell the engine to load?")

# CHARCUST maps race/class to model hash IDs
# Compare Frontiers CHARCUST vs Original CHARCUST
for label, path in [("Frontiers", "workspace/CHARCUST_Frontiers.ESF"), 
                    ("Original", "workspace/CHARCUST_Original.ESF")]:
    if os.path.exists(path):
        with open(path, 'rb') as f:
            cust_data = f.read()
        # Find model hash references inside CHARCUST
        # These are typically 4-byte values matching our target hashes
        found = []
        for t in targets:
            h = int(t['expansion_hash'], 16)
            h_bytes = struct.pack('<I', h)
            count = 0
            pos = 0
            while True:
                pos = cust_data.find(h_bytes, pos)
                if pos == -1: break
                count += 1
                pos += 1
            if count > 0:
                found.append((t['expansion_hash'], count))
        print(f"  {label} CHARCUST references to our 11 hashes: {len(found)} hashes found, total {sum(c for _,c in found)} refs")
        for h, c in found:
            print(f"    {h}: {c} references")
    else:
        print(f"  {label} CHARCUST: FILE NOT FOUND")

print()

# ─── Step 6: Is the patched ISO using the ORIGINAL CHARCUST or FRONTIERS? ───
print(f"[Step 6] Checking which CHARCUST.CSF is in the patched ISO...")

PATCHED_ISO = 'EQOA_Frontiers_Patched.iso'
with open(PATCHED_ISO, 'rb') as f:
    dir_data = f.read(1024 * 1024)

# Find CHARCUST.CSF in the directory
for fname in [b'CHARCUST.CSF;1', b'CHARCUST.ESF;1']:
    idx = dir_data.find(fname)
    if idx != -1:
        # Try to figure out the name length byte
        for name_len_offset in [33, 34, 35]:
            try_start = idx - name_len_offset
            lba = struct.unpack_from('<I', dir_data, try_start + 2)[0]
            lba2 = struct.unpack_from('>I', dir_data, try_start + 6)[0]
            sz = struct.unpack_from('<I', dir_data, try_start + 10)[0]
            if lba == lba2 and 0 < lba < 2000000 and sz > 0:
                print(f"  {fname.decode()}: LBA={lba}, Size={sz:,}")
                break
