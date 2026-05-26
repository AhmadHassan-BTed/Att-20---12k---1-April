#!/usr/bin/env python3
"""
check_baseline.py
=================
The most basic question nobody has asked:
Does the UNMODIFIED Frontiers ISO show characters?

If characters are invisible in the UNMODIFIED Frontiers ISO too, then our
CHAR.ESF changes are irrelevant — the problem is something else entirely
(server connection, DNAS, game state, etc.)

This script compares the PATCHED ISO against the UNMODIFIED to identify
EVERY file that differs, not just CHAR.ESF. Something else might be broken.
"""
import struct, os

FRONTIERS_ISO = 'EQOA_Frontiers.iso'
PATCHED_ISO   = 'EQOA_Frontiers_Patched.iso'

print("=" * 70)
print("  BASELINE COMPARISON: Every file difference between ISOs")
print("=" * 70)
print()

# Read directory listings from both ISOs
def read_iso_directory(iso_path):
    """Read ISO9660 directory records."""
    files = {}
    with open(iso_path, 'rb') as f:
        # Read PVD at sector 16
        f.seek(16 * 2048)
        pvd = f.read(2048)
        if pvd[:6] != b'\x01CD001':
            print(f"  [{iso_path}] Invalid PVD!")
            return files
        
        vol_space = struct.unpack_from('<I', pvd, 80)[0]
        
        # Root directory record at PVD offset 156
        root_lba  = struct.unpack_from('<I', pvd, 158)[0]
        root_size = struct.unpack_from('<I', pvd, 166)[0]
        
        # Read root directory
        f.seek(root_lba * 2048)
        root_dir = f.read(root_size)
        
        pos = 0
        while pos < len(root_dir):
            rec_len = root_dir[pos]
            if rec_len == 0:
                # Move to next sector boundary
                next_sector = ((pos // 2048) + 1) * 2048
                if next_sector >= len(root_dir):
                    break
                pos = next_sector
                continue
            
            if pos + rec_len > len(root_dir):
                break
            
            lba  = struct.unpack_from('<I', root_dir, pos + 2)[0]
            size = struct.unpack_from('<I', root_dir, pos + 10)[0]
            name_len = root_dir[pos + 32]
            
            if name_len > 0 and pos + 33 + name_len <= len(root_dir):
                name = root_dir[pos+33 : pos+33+name_len].decode('ascii', errors='replace')
                if name not in ('\x00', '\x01'):
                    files[name] = {'lba': lba, 'size': size}
            
            pos += rec_len
        
        files['__VOL_SPACE__'] = vol_space
    
    return files

print(f"[*] Reading directory from {FRONTIERS_ISO}...")
front_files = read_iso_directory(FRONTIERS_ISO)

print(f"[*] Reading directory from {PATCHED_ISO}...")
patch_files = read_iso_directory(PATCHED_ISO)

front_vol = front_files.pop('__VOL_SPACE__', 0)
patch_vol = patch_files.pop('__VOL_SPACE__', 0)

print(f"\n  Frontiers volume: {front_vol} sectors ({front_vol*2048:,} bytes)")
print(f"  Patched volume  : {patch_vol} sectors ({patch_vol*2048:,} bytes)")
print(f"  Delta           : {patch_vol - front_vol:+d} sectors ({(patch_vol-front_vol)*2048:+,} bytes)")
print()

# Compare files
all_names = sorted(set(list(front_files.keys()) + list(patch_files.keys())))

print(f"  {'Filename':<20} {'Front LBA':>10} {'Front Size':>12} {'Patch LBA':>10} {'Patch Size':>12} {'Status'}")
print(f"  {'-'*20} {'-'*10} {'-'*12} {'-'*10} {'-'*12} {'-'*10}")

changed_files = []
for name in all_names:
    f_info = front_files.get(name)
    p_info = patch_files.get(name)
    
    if f_info and p_info:
        if f_info['lba'] != p_info['lba'] or f_info['size'] != p_info['size']:
            status = "CHANGED"
            changed_files.append(name)
        else:
            status = "same"
        print(f"  {name:<20} {f_info['lba']:>10} {f_info['size']:>12,} {p_info['lba']:>10} {p_info['size']:>12,} {status}")
    elif f_info and not p_info:
        print(f"  {name:<20} {f_info['lba']:>10} {f_info['size']:>12,} {'MISSING':>10} {'':>12} REMOVED!")
        changed_files.append(name)
    elif not f_info and p_info:
        print(f"  {name:<20} {'NEW':>10} {'':>12} {p_info['lba']:>10} {p_info['size']:>12,} ADDED!")
        changed_files.append(name)

print()
print(f"  Total files in Frontiers: {len(front_files)}")
print(f"  Total files in Patched : {len(patch_files)}")
print(f"  Changed/Added/Removed  : {len(changed_files)}")

if changed_files:
    print(f"\n  Changed files: {', '.join(changed_files)}")
    if 'CHAR.ESF;1' in changed_files and len(changed_files) == 1:
        print("  >>> ONLY CHAR.ESF was changed — repack_iso.py only touched CHAR.ESF <<<")
    elif len(changed_files) > 1:
        print("  >>> MULTIPLE files changed — check if CHARCUST/CHARSEL were updated <<<")

# ─── Key question: Do the ISO differ on CHARCUST, CHARSEL, or CHARFACE? ─────
print("\n[*] Critical dependency check:")
for dep in ['CHARCUST.CSF;1', 'CHARFACE.CSF;1', 
            'CHARSEL1.CSF;1', 'CHARSEL2.CSF;1', 
            'CHARSEL3.CSF;1', 'CHARSEL4.CSF;1',
            'SLUS_207.44;1']:
    f_info = front_files.get(dep)
    p_info = patch_files.get(dep)
    if f_info and p_info:
        same = f_info['lba'] == p_info['lba'] and f_info['size'] == p_info['size']
        print(f"  {dep:<20} {'SAME' if same else 'DIFFERENT!'}")
    else:
        print(f"  {dep:<20} {'missing in one or both'}")
