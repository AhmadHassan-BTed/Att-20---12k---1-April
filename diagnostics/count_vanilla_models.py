import os
from esf_parser import ESFParser
from collections import Counter

print("[*] Parsing Vanilla CHAR.ESF...")
with open('workspace/original/CHAR.ESF', 'rb') as f:
    data = f.read()

parser = ESFParser(data).parse()
print(f"[+] Total entries in Vanilla: {len(parser.pointer_table)}")

type_counts = Counter()
sizes = []
for entry in parser.pointer_table:
    type_counts[entry.type_id] += 1
    if entry.type_id == 0x62700:
        sizes.append(entry.length)

print("\n=== Type Distribution in Vanilla CHAR.ESF ===")
for tid, cnt in sorted(type_counts.items(), key=lambda x: x[1], reverse=True):
    print(f"  Type 0x{tid:05X}: {cnt} entries")

print(f"\nTotal 0x62700 models: {len(sizes)}")
print(f"  - Large models (>400KB, base bodies):  {sum(1 for s in sizes if s > 400000)}")
print(f"  - Medium models (100KB-400KB):        {sum(1 for s in sizes if 100000 <= s <= 400000)}")
print(f"  - Small models (<100KB, armor/parts): {sum(1 for s in sizes if s < 100000)}")
