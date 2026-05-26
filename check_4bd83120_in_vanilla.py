import os
from esf_parser import ESFParser

with open('workspace/original/CHAR.ESF', 'rb') as f:
    orig_data = f.read()
orig_parser = ESFParser(orig_data).parse()

found = False
for entry in orig_parser.pointer_table:
    if entry.asset_id == 0x4BD83120:
        print(f"[+] Found 0x4BD83120 in Vanilla CHAR.ESF: {entry}")
        found = True
        break

if not found:
    print("[-] 0x4BD83120 NOT found in Vanilla CHAR.ESF")
