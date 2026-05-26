import os
from esf_parser import ESFParser

print("[*] Parsing original/Vanilla CHAR.ESF...")
with open('workspace/original/CHAR.ESF', 'rb') as f:
    orig_data = f.read()
orig_parser = ESFParser(orig_data).parse()

print("[*] Parsing expansion/Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

orig_hashes = {entry.asset_id: entry for entry in orig_parser.pointer_table if entry.asset_id is not None}
exp_hashes = {entry.asset_id: entry for entry in exp_parser.pointer_table if entry.asset_id is not None}

shared_hashes = []
for h in orig_hashes:
    if h in exp_hashes:
        shared_hashes.append(h)

print("\n" + "="*50)
print(f"SHARED ASSET ANALYSIS")
print("="*50)
print(f"Vanilla unique assets:     {len(orig_hashes)}")
print(f"Frontiers unique assets:   {len(exp_hashes)}")
print(f"Shared assets in BOTH:     {len(shared_hashes)}")

# Categorize shared assets by their Vanilla type
from collections import Counter
type_counts = Counter()
for h in shared_hashes:
    type_counts[orig_hashes[h].type_id] += 1

print("\n=== Type Distribution of Shared Assets ===")
for tid, cnt in sorted(type_counts.items(), key=lambda x: x[1], reverse=True):
    print(f"  Type 0x{tid:05X}: {cnt} entries")
