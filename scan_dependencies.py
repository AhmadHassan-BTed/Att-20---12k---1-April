import os, sys, struct
from esf_parser import ESFParser

print("[*] Loading Original CHAR.ESF to map all assets...")
with open('workspace/original/CHAR.ESF', 'rb') as f:
    orig_data = f.read()

parser = ESFParser(orig_data).parse()
full_esf_map = {entry.asset_id: entry for entry in parser.pointer_table if entry.asset_id is not None}
print(f"[+] Mapped {len(full_esf_map)} assets from original ESF.")

print("\n[*] Auditing 11 Master Payloads for missed dependencies...")
import glob
bin_files = sorted(glob.glob('workspace/payloads/*.bin'))

# First, map what has actually been extracted
extracted_hashes = {int(os.path.basename(f).split('_')[1].split('.')[0], 16) for f in bin_files}

missed_dependencies = {}

for filepath in bin_files:
    filename = os.path.basename(filepath)
    asset_hash = int(filename.split('_')[1].split('.')[0], 16)
    
    # We only want to audit master character payloads (type 0x72700 / 0x62700)
    with open(filepath, 'rb') as f:
        data = f.read()
    if len(data) < 12:
        continue
    type_id = struct.unpack_from('<I', data, 0)[0]
    if type_id not in (0x62700, 0x72700):
        continue
        
    print(f"\n[*] Auditing {filename} (size {len(data):,} bytes):")
    
    # Scan the ENTIRE payload for any 32-bit values that exist in our full_esf_map
    # but were NOT extracted (i.e. not in extracted_hashes)
    for i in range(0, len(data) - 4, 4):
        val = struct.unpack_from('<I', data, i)[0]
        if val in full_esf_map and val not in extracted_hashes:
            if val not in missed_dependencies:
                missed_dependencies[val] = []
            missed_dependencies[val].append((filename, i))
            print(f"    -> Found MISSED dependency at offset 0x{i:X}: 0x{val:08X} (Type: 0x{full_esf_map[val].type_id:05X})")

print("\n" + "="*50)
print(f"AUDIT COMPLETE: Found {len(missed_dependencies)} unique missed dependencies!")
print("="*50)
for h, refs in missed_dependencies.items():
    ref_str = ", ".join(f"{r[0]}@0x{r[1]:X}" for r in refs[:5])
    print(f"  Missed: 0x{h:08X} | Type: 0x{full_esf_map[h].type_id:05X} | Size: {full_esf_map[h].length:,} B | Referenced by: {ref_str}")
