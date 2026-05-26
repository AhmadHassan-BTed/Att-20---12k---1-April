import os, struct, glob

payload_dir = './workspace/payloads'
bin_files = glob.glob(os.path.join(payload_dir, '*.bin'))

types = {}
for filepath in bin_files:
    with open(filepath, 'rb') as f:
        data = f.read()
    if len(data) >= 12:
        type_id = struct.unpack_from('<I', data, 0)[0]
        types[type_id] = types.get(type_id, 0) + 1

print(f"Total payload files: {len(bin_files)}")
print("Type distribution:")
for tid, count in sorted(types.items(), key=lambda x: x[1], reverse=True):
    print(f"  0x{tid:05X}: {count} files")
