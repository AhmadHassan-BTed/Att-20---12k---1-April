from esf_parser import ESFParser

with open('workspace/original/CHAR.ESF', 'rb') as f:
    orig_data = f.read()
orig_parser = ESFParser(orig_data).parse()

with open('workspace/expansion/CHAR.ESF', 'rb') as f:
    exp_data = f.read()
exp_parser = ESFParser(exp_data).parse()

van_entry = None
for entry in orig_parser.pointer_table:
    if entry.asset_id == 0x4BD83120:
        van_entry = entry
        break

fro_entry = None
for entry in exp_parser.pointer_table:
    if entry.asset_id == 0x4BD83120:
        fro_entry = entry
        break

van_bytes = orig_data[van_entry.offset : van_entry.offset + van_entry.length]
fro_bytes = exp_data[fro_entry.offset : fro_entry.offset + fro_entry.length]

if van_bytes == fro_bytes:
    print("[PASS] 0x4BD83120 is 100% byte-for-byte identical between Vanilla and Frontiers!")
else:
    print("[-] 0x4BD83120 is DIFFERENT!")
    # Let's find how many bytes differ and the first diff offset
    diffs = 0
    first_diff = -1
    for i in range(min(len(van_bytes), len(fro_bytes))):
        if van_bytes[i] != fro_bytes[i]:
            diffs += 1
            if first_diff == -1:
                first_diff = i
    print(f"  Total different bytes: {diffs}")
    print(f"  First difference at byte offset: 0x{first_diff:X}")
