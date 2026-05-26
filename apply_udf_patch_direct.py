#!/usr/bin/env python3
"""
apply_udf_patch_direct.py
=========================
Direct binary patch of the UDF allocation descriptor for CHAR.ESF.
Uses low-level file access to bypass Windows file locks.

Target: EQOA_Frontiers_Patched.iso
Patch:  Offset 0xA8934 (8 bytes) - UDF allocation descriptor
        Old: 1c f6 d7 08  e4 0c 00 00  (size=148370972, logical_lba=3300)
        New: 8c 6e d9 08  7a c4 16 00  (size=148467340, logical_lba=1492090)

Also patches:
        Offset 0xA8838 (8 bytes) - UDF File Entry Information Length field
        Old: 1c f6 d7 08 00 00 00 00
        New: 8c 6e d9 08 00 00 00 00
"""
import struct, os, sys, ctypes

ISO_PATH = 'EQOA_Frontiers_Patched.iso'

# The exact bytes we're changing
OLD_AD    = bytes.fromhex('1cf6d708e40c0000')  # at 0xA8934
NEW_SIZE  = 148_467_340
NEW_LGBA  = 1_492_090  # = 1492368 - 278 (partition offset)

new_sz_le  = struct.pack('<I', NEW_SIZE)
new_lba_le = struct.pack('<I', NEW_LGBA)
NEW_AD     = new_sz_le + new_lba_le

# Also update Information Length field
OLD_IL = struct.pack('<Q', 148_370_972)  # at 0xA8838 (FE + 0x38)
NEW_IL = struct.pack('<Q', NEW_SIZE)

# Also update Logical Blocks Recorded
OLD_BL = struct.pack('<Q', 72447)  # at 0xA8840 (FE + 0x40)
NEW_BL = struct.pack('<Q', (NEW_SIZE + 2047) // 2048)

print("=" * 60)
print("  DIRECT UDF PATCH APPLICATION")
print("=" * 60)
print(f"  ISO   : {ISO_PATH}")
print(f"  Offset: 0xA8934 (Allocation Descriptor)")
print(f"  Old AD: {OLD_AD.hex()}")
print(f"  New AD: {NEW_AD.hex()}")
print()
print(f"  Offset: 0xA8838 (Information Length)")
print(f"  Old IL: {OLD_IL.hex()}")
print(f"  New IL: {NEW_IL.hex()}")
print()

if not os.path.exists(ISO_PATH):
    print(f"[-] ISO not found: {ISO_PATH}")
    sys.exit(1)

# Try using os.open with os.O_RDWR | os.O_BINARY for lower-level access
import os

def patch_at_offset(path, offset, old_bytes, new_bytes, label):
    fd = None
    try:
        fd = os.open(path, os.O_RDWR | os.O_BINARY)
        os.lseek(fd, offset, os.SEEK_SET)
        current = os.read(fd, len(old_bytes))
        print(f"  [{label}] Current at 0x{offset:X}: {current.hex()}")
        if current != old_bytes:
            print(f"  [{label}] WARNING: expected {old_bytes.hex()}")
            if current == new_bytes:
                print(f"  [{label}] Already patched!")
                return True
            # If the first half matches (size was already patched), patch just LBA
            if len(old_bytes) == 8 and current[:4] == new_bytes[:4]:
                print(f"  [{label}] Size already patched, patching LBA only...")
                os.lseek(fd, offset + 4, os.SEEK_SET)
                os.write(fd, new_bytes[4:])
                return True
            print(f"  [{label}] Unexpected content, skipping to be safe.")
            return False
        os.lseek(fd, offset, os.SEEK_SET)
        os.write(fd, new_bytes)
        print(f"  [{label}] Patched: {old_bytes.hex()} -> {new_bytes.hex()}")
        return True
    except PermissionError:
        print(f"  [{label}] PERMISSION DENIED. The ISO file is locked by another process.")
        print(f"           Please close PCSX2 (or any program using this ISO) and retry.")
        return False
    except Exception as e:
        print(f"  [{label}] Error: {e}")
        return False
    finally:
        if fd is not None:
            try: os.close(fd)
            except: pass

# ── Patch 1: Allocation Descriptor at 0xA8934 ─────────────────────────────────
ok1 = patch_at_offset(ISO_PATH, 0xA8934, OLD_AD, NEW_AD, "AllocDesc")
print()

# ── Patch 2: Information Length at 0xA8838 ────────────────────────────────────
ok2 = patch_at_offset(ISO_PATH, 0xA8838, OLD_IL, NEW_IL, "InfoLen")
print()

# ── Patch 3: Logical Blocks at 0xA8840 ────────────────────────────────────────
ok3 = patch_at_offset(ISO_PATH, 0xA8840, OLD_BL, NEW_BL, "LogBlocks")
print()

# ── Verify ────────────────────────────────────────────────────────────────────
if ok1 and ok2 and ok3:
    print("[*] Verifying all patches...")
    try:
        fd = os.open(ISO_PATH, os.O_RDONLY | os.O_BINARY)
        
        os.lseek(fd, 0xA8934, os.SEEK_SET)
        v1 = os.read(fd, 8)
        
        os.lseek(fd, 0xA8838, os.SEEK_SET)
        v2 = os.read(fd, 8)

        os.lseek(fd, 0xA8840, os.SEEK_SET)
        v3 = os.read(fd, 8)
        
        os.close(fd)
        
        r1 = "OK" if v1 == NEW_AD else f"FAIL (got {v1.hex()})"
        r2 = "OK" if v2 == NEW_IL else f"FAIL (got {v2.hex()})"
        r3 = "OK" if v3 == NEW_BL else f"FAIL (got {v3.hex()})"
        
        print(f"  AllocDesc  (0xA8934): {r1}")
        print(f"  InfoLen    (0xA8838): {r2}")
        print(f"  LogBlocks  (0xA8840): {r3}")
        
        if all(x == "OK" for x in [r1, r2, r3]):
            print()
            print("=" * 60)
            print("  [SUCCESS] ALL UDF PATCHES APPLIED!")
            print("  The PS2 IOP will now load CHAR.ESF from LBA 1492368.")
            print("=" * 60)
        else:
            print("\n  [PARTIAL] Some patches may not have taken effect.")
    except Exception as e:
        print(f"  Verification error: {e}")
else:
    print("=" * 60)
    print("  [BLOCKED] Could not write to ISO. Please close any programs")
    print("  that have the ISO open (PCSX2, etc.) and run this script again.")
    print("=" * 60)
    sys.exit(1)
