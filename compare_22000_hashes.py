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

van_22000 = [e for e in orig_parser.pointer_table if e.type_id == 0x22000]
fro_22000 = [e for e in exp_parser.pointer_table if e.type_id == 0x22000]

print(f"\nVanilla has {len(van_22000)} entries of type 0x22000:")
for entry in van_22000:
    print(f"  - ID: 0x{entry.asset_id:08X} | Size: {entry.length:,} B")

print(f"\nFrontiers has {len(fro_22000)} entries of type 0x22000:")
for entry in fro_22000:
    print(f"  - ID: 0x{entry.asset_id:08X} | Size: {entry.length:,} B")
