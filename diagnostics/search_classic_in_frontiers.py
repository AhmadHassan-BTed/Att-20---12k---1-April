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

# Get the list of our 11 target Vanilla hashes and their sizes
import json
with open('workspace/target_assets.json', 'r') as f:
    targets = json.load(f)

print("\n=== Auditing Targets vs Native Frontiers 0x62700 assets ===")
print(f"Target count: {len(targets)}")
print(f"Frontiers native 0x62700 entries: {sum(1 for e in exp_parser.pointer_table if e.type_id == 0x62700)}")

# Find any native Frontiers entries of type 0x62700 or 0x72700 that are extremely close in size to our targets!
# This would indicate the model was already ported or present in a different form.
for t in targets:
    t_size = t['original_length']
    t_hash = t['original_hash']
    print(f"\nTarget: {t_hash} | Size: {t_size:,} B")
    
    matches = []
    for entry in exp_parser.pointer_table:
        # Check size similarity (within 5%)
        pct_diff = abs(entry.length - t_size) / t_size
        if pct_diff < 0.05:
            matches.append(entry)
            
    if matches:
        print("  Found size-similar assets in Frontiers:")
        for m in matches[:5]:
            print(f"    - ID: 0x{m.asset_id:08X} | Size: {m.length:,} B (diff: {abs(m.length - t_size)/t_size:.2%}) | Type: 0x{m.type_id:05X}")
    else:
        print("  No size-similar assets found.")
