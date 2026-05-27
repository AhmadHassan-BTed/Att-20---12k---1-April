#!/usr/bin/env python3
"""
Verify that patched data was correctly written to ISO at the target LBA.
Compares first 32 bytes of patch with what's actually in the ISO.
"""
import struct

def check_iso_appended_bytes():
    print("\n[CHECK] ISO Appended Bytes Verification")
    print("=" * 70)

    try:
        # Read expected header from FINAL_CHAR_MERGED.ESF
        with open('workspace/FINAL_CHAR_MERGED.ESF', 'rb') as f:
            expected_header = f.read(32)

        # Read from ISO at the LBA where CHAR.ESF should be
        # CHAR.ESF in ISO is at LBA 1492368, which is byte offset 1492368 * 2048
        lba = 1492368
        offset = lba * 2048

        with open('iso/patched/EQOA_Frontiers_Patched.iso', 'rb') as f:
            f.seek(offset)
            actual_header = f.read(32)

        print(f"\nLBA: {lba} (byte offset: {offset:,})")
        print(f"\nExpected header (first 32 bytes of FINAL_CHAR_MERGED.ESF):")
        print(f"  {expected_header.hex()}")
        print(f"\nActual header (from ISO at LBA {lba}):")
        print(f"  {actual_header.hex()}")

        if expected_header == actual_header:
            print(f"\n[OK] Headers MATCH - patch was written correctly!")
            return True
        else:
            print(f"\n[ERROR] Headers DO NOT MATCH - patch may not have been written!")
            return False

    except Exception as e:
        print(f"[ERROR] {e}")
        return False

if __name__ == '__main__':
    check_iso_appended_bytes()
