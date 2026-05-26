import os
from esf_parser import ESFParser

def get_entry(esf_path, target_hash):
    if not os.path.exists(esf_path):
        return None
    with open(esf_path, 'rb') as f:
        data = f.read()
    parser = ESFParser(data).parse()
    for entry in parser.pointer_table:
        if entry.asset_id == target_hash:
            return entry
    return None

target_hash = 0x2EF8E480

print(f"=== AUDITING POINTER ENTRIES FOR HASH 0x{target_hash:08X} ===")

original = get_entry('workspace/original/CHAR.ESF', target_hash)
print(f"Original ESF:  {original}")

expansion = get_entry('workspace/expansion/CHAR.ESF', target_hash)
print(f"Expansion ESF: {expansion}")

patched = get_entry('workspace/FINAL_CHAR_MERGED.ESF', target_hash)
print(f"Patched ESF:   {patched}")
