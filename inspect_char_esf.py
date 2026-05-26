import os
from esf_parser import ESFParser
from collections import Counter

print("[*] Parsing Frontiers CHAR.ESF...")
with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    data = f.read()

parser = ESFParser(data).parse()
print(f"[+] Total entries: {len(parser.pointer_table)}")

type_counts = Counter()
for entry in parser.pointer_table:
    type_counts[entry.type_id] += 1

print("\n=== Type Distribution in Frontiers CHAR.ESF ===")
for tid, cnt in sorted(type_counts.items(), key=lambda x: x[1], reverse=True):
    print(f"  Type 0x{tid:05X}: {cnt} entries")
