import struct

with open('workspace/SLUS_207.44', 'rb') as f:
    elf_data = f.read()

# Target offset: 0x07E9BB45
target_offset = 0x07E9BB45
# Target hash: 0x2EF8E480
target_hash = 0x2EF8E480

offset_occurrences = []
hash_occurrences = []

for i in range(0, len(elf_data) - 4, 4):
    val = struct.unpack_from('<I', elf_data, i)[0]
    if val == target_offset:
        offset_occurrences.append(i)
    if val == target_hash:
        hash_occurrences.append(i)

print(f"Target Offset 0x{target_offset:08X} ({target_offset}): found at ELF offsets {offset_occurrences}")
print(f"Target Hash   0x{target_hash:08X}: found at ELF offsets {hash_occurrences}")
