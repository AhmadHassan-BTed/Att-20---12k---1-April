import struct

with open('workspace/SLUS_207.44', 'rb') as f:
    elf_data = f.read()

# Original LBA of CHAR.ESF: 3578 (0x0DFB)
# Original Size of CHAR.ESF: 148370972 (0x08D7F61C)
target_lba = 3578
target_size = 148370972

lba_matches = []
size_matches = []

for i in range(0, len(elf_data) - 4, 4):
    val = struct.unpack_from('<I', elf_data, i)[0]
    if val == target_lba:
        lba_matches.append(i)
    if val == target_size:
        size_matches.append(i)

print(f"Target LBA  3578 (0x0DFB): found at ELF offsets {lba_matches}")
print(f"Target Size 148370972 (0x08D7F61C): found at ELF offsets {size_matches}")
