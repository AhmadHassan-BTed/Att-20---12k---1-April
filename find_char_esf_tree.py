#!/usr/bin/env python3
"""
THE critical question: Where is CHAR.ESF stored inside the Frontiers ISO?
It's NOT in the root directory. It's inside a subdirectory (DATA? DATA2? DATA3?).
Let's find it by parsing subdirectories.

Also: After all our patches, which LBA does the game ACTUALLY read?
"""
import struct, os

def parse_directory(data, f, iso_data_cache=None):
    """Parse an ISO9660 directory and return file entries."""
    entries = []
    pos = 0
    while pos < len(data):
        rec_len = data[pos]
        if rec_len == 0:
            next_sector = ((pos // 2048) + 1) * 2048
            if next_sector >= len(data):
                break
            pos = next_sector
            continue
        
        if pos + rec_len > len(data):
            break
        
        flags = data[pos + 25]
        is_dir = (flags & 0x02) != 0
        lba  = struct.unpack_from('<I', data, pos + 2)[0]
        size = struct.unpack_from('<I', data, pos + 10)[0]
        name_len = data[pos + 32]
        
        if name_len > 0 and pos + 33 + name_len <= len(data):
            name = data[pos+33 : pos+33+name_len].decode('ascii', errors='replace')
            if name not in ('\x00', '\x01'):
                entries.append({
                    'name': name,
                    'lba': lba,
                    'size': size,
                    'is_dir': is_dir,
                    'offset': pos
                })
        
        pos += rec_len
    
    return entries

def find_all_files(iso_path, label):
    """Recursively find all files in ISO9660 directory structure."""
    all_files = {}
    
    with open(iso_path, 'rb') as f:
        # Read PVD
        f.seek(16 * 2048)
        pvd = f.read(2048)
        root_lba  = struct.unpack_from('<I', pvd, 158)[0]
        root_size = struct.unpack_from('<I', pvd, 166)[0]
        
        # Parse root directory
        f.seek(root_lba * 2048)
        root_dir = f.read(root_size)
        root_entries = parse_directory(root_dir, f)
        
        for entry in root_entries:
            path = '/' + entry['name']
            if entry['is_dir']:
                # Read subdirectory
                f.seek(entry['lba'] * 2048)
                sub_dir = f.read(entry['size'])
                sub_entries = parse_directory(sub_dir, f)
                for sub in sub_entries:
                    sub_path = path + '/' + sub['name']
                    all_files[sub_path] = sub
                    if sub['is_dir']:
                        # Read sub-subdirectory (one more level)
                        f.seek(sub['lba'] * 2048)
                        subsub_dir = f.read(sub['size'])
                        subsub_entries = parse_directory(subsub_dir, f)
                        for subsub in subsub_entries:
                            all_files[path + '/' + sub['name'] + '/' + subsub['name']] = subsub
            else:
                all_files[path] = entry
    
    return all_files

print("=" * 70)
print("  COMPLETE FILE TREE COMPARISON")
print("=" * 70)
print()

print("[*] Scanning EQOA_Frontiers.iso (unmodified)...")
front_files = find_all_files('EQOA_Frontiers.iso', 'Frontiers')

print("[*] Scanning EQOA_Frontiers_Patched.iso...")
patch_files = find_all_files('EQOA_Frontiers_Patched.iso', 'Patched')

print()

# Find CHAR.ESF specifically
char_esf_paths = [p for p in front_files if 'CHAR.ESF' in p.upper()]
print(f"[*] CHAR.ESF locations in Frontiers:")
for p in sorted(char_esf_paths):
    f = front_files[p]
    print(f"  {p}: LBA={f['lba']}, Size={f['size']:,}")

print()
char_esf_paths_p = [p for p in patch_files if 'CHAR.ESF' in p.upper()]
print(f"[*] CHAR.ESF locations in Patched:")
for p in sorted(char_esf_paths_p):
    f = patch_files[p]
    print(f"  {p}: LBA={f['lba']}, Size={f['size']:,}")

print()

# Compare ALL files for differences
print("[*] Files that differ between Frontiers and Patched:")
all_paths = sorted(set(list(front_files.keys()) + list(patch_files.keys())))
changed = 0
for p in all_paths:
    ff = front_files.get(p)
    pf = patch_files.get(p)
    if ff and pf:
        if ff['lba'] != pf['lba'] or ff['size'] != pf['size']:
            print(f"  CHANGED: {p}")
            print(f"    Front: LBA={ff['lba']}, Size={ff['size']:,}")
            print(f"    Patch: LBA={pf['lba']}, Size={pf['size']:,}")
            changed += 1
    elif ff and not pf:
        print(f"  REMOVED: {p} (was LBA={ff['lba']}, Size={ff['size']:,})")
        changed += 1
    elif not ff and pf:
        print(f"  ADDED:   {p} (LBA={pf['lba']}, Size={pf['size']:,})")
        changed += 1

if changed == 0:
    print("  NO DIFFERENCES! The patched ISO has identical directory structure!")
    print("  >>> The repack_iso.py only patched the raw CHAR.ESF directory record,")
    print("  >>> but the subdirectory entry may not have been found/updated! <<<")

print()

# Also find ALL character-related files
print("[*] All character-related files in Frontiers:")
for p in sorted(front_files.keys()):
    if any(k in p.upper() for k in ['CHAR', 'CUST', 'FACE', 'SEL']):
        f = front_files[p]
        print(f"  {p}: LBA={f['lba']}, Size={f['size']:,}, dir={f['is_dir']}")
