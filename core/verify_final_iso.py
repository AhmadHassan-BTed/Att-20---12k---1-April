import os

iso_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
esf_path = 'workspace/FINAL_CHAR_MERGED.ESF'

print(f"[*] Commencing direct binary sector verification...")
print(f"    Patched ISO: {iso_path}")
print(f"    Merged ESF:  {esf_path}")

if not os.path.exists(iso_path):
    print(f"[-] Error: Patched ISO not found!")
    exit(1)
    
if not os.path.exists(esf_path):
    print(f"[-] Error: Merged ESF not found!")
    exit(1)

# Expected parameters from repack logs
lba = 1492368
esf_size = os.path.getsize(esf_path)
start_offset = lba * 2048

print(f"    - Target LBA:        {lba}")
print(f"    - Target Byte Offset: 0x{start_offset:X}")
print(f"    - Expected Size:     {esf_size:,} bytes")

with open(iso_path, 'rb') as iso_f:
    iso_f.seek(start_offset)
    appended_data = iso_f.read(esf_size)

with open(esf_path, 'rb') as esf_f:
    original_data = esf_f.read()

if len(appended_data) != len(original_data):
    print(f"[-] Error: Size mismatch! Read {len(appended_data):,} bytes from ISO, but ESF has {len(original_data):,} bytes.")
    exit(1)

if appended_data == original_data:
    print("\n[PASS] ISO SECTOR INTEGRITY VERIFICATION SUCCESSFUL!")
    print("       The data written at LBA sector 1,492,368 is an exact, byte-for-byte match to the merged ESF!")
else:
    # Find first diff
    for i in range(len(original_data)):
        if appended_data[i] != original_data[i]:
            print(f"[-] Error: Data mismatch at byte offset 0x{i:X} inside ESF payload!")
            break
    exit(1)
