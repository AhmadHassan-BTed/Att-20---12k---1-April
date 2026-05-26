#!/usr/bin/env python3
"""Compare pointer tables between original Frontiers CHAR.ESF and merged."""
from esf_parser import ESFParser
import struct, os

# Parse original Frontiers CHAR.ESF
with open('EQOA_Frontiers.iso', 'rb') as f:
    d = f.read(1024*1024)
idx = d.find(b'CHAR.ESF;1')
dr = idx - 33
lba = struct.unpack_from('<I', d, dr+2)[0]
sz = struct.unpack_from('<I', d, dr+10)[0]
with open('EQOA_Frontiers.iso', 'rb') as f:
    f.seek(lba * 2048)
    orig = f.read(sz)

parser = ESFParser(orig)
parser.parse()
print("Original Frontiers CHAR.ESF:")
print(f"  Root Children: {parser.root['child_count']}")
print(f"  Pointer Table: {len(parser.pointer_table)} entries")
print(f"  First 3:")
for e in parser.pointer_table[:3]:
    print(f"    {e}")
print(f"  Last 3:")
for e in parser.pointer_table[-3:]:
    print(f"    {e}")

types = {}
for e in parser.pointer_table:
    t = hex(e.type_id)
    types[t] = types.get(t, 0) + 1
print(f"  Types: {types}")
print()

# Parse merged
with open('workspace/FINAL_CHAR_MERGED.ESF', 'rb') as f:
    merged = f.read()
parser2 = ESFParser(merged)
parser2.parse()
print("Merged CHAR.ESF:")
print(f"  Root Children: {parser2.root['child_count']}")
print(f"  Pointer Table: {len(parser2.pointer_table)} entries")
types2 = {}
for e in parser2.pointer_table:
    t = hex(e.type_id)
    types2[t] = types2.get(t, 0) + 1
print(f"  Types: {types2}")
print()

# Compare hashes
orig_hashes = {e.asset_id for e in parser.pointer_table}
merg_hashes = {e.asset_id for e in parser2.pointer_table}
print(f"  Hashes only in original: {len(orig_hashes - merg_hashes)}")
print(f"  Hashes only in merged:   {len(merg_hashes - orig_hashes)}")
print(f"  Hashes in both:          {len(orig_hashes & merg_hashes)}")
print()

# For each of our 11 targets, compare entries
import json
with open('workspace/target_assets.json') as f:
    targets = json.load(f)

print("Target asset comparison:")
for t in targets:
    h = int(t['expansion_hash'], 16)
    orig_entry = None
    merg_entry = None
    for e in parser.pointer_table:
        if e.asset_id == h:
            orig_entry = e
    for e in parser2.pointer_table:
        if e.asset_id == h:
            merg_entry = e
    
    print(f"  {t['expansion_hash']}:")
    if orig_entry:
        print(f"    Original: type=0x{orig_entry.type_id:05X}, offset={orig_entry.offset:,}, len={orig_entry.length:,}")
    else:
        print(f"    Original: NOT FOUND via hash!")
    if merg_entry:
        print(f"    Merged:   type=0x{merg_entry.type_id:05X}, offset={merg_entry.offset:,}, len={merg_entry.length:,}")
    else:
        print(f"    Merged:   NOT FOUND via hash!")
