def inspect(iso_path):
    print(f"=== INSPECTING OCCURRENCES IN {iso_path} ===")
    with open(iso_path, 'rb') as f:
        data = f.read()
    
    import re
    matches = [m.start() for m in re.finditer(b'CHAR.ESF', data, re.IGNORECASE)]
    
    for idx, pos in enumerate(matches):
        surround_32 = data[max(0, pos-32) : pos]
        name = data[pos : pos+12]
        print(f"Occurrence {idx} at 0x{pos:X}:")
        print(f"  Name: {name}")
        print(f"  Preceding 32 bytes Hex: {surround_32.hex()}")
        # Decode LBA and size
        if len(surround_32) >= 18:
            # dr_start is pos - 32
            # LBA is at offset 2 (4 bytes LE) and 6 (4 bytes BE)
            # Size is at offset 10 (4 bytes LE) and 14 (4 bytes BE)
            import struct
            lba_le = struct.unpack_from('<I', surround_32, 2)[0]
            lba_be = struct.unpack_from('>I', surround_32, 6)[0]
            size_le = struct.unpack_from('<I', surround_32, 10)[0]
            size_be = struct.unpack_from('>I', surround_32, 14)[0]
            print(f"  Unpacked: LBA_LE={lba_le}, LBA_BE={lba_be}, Size_LE={size_le}, Size_BE={size_be}")

print("--- Patched ISO ---")
inspect('EQOA_Frontiers_Patched.iso')

print("\n--- Original ISO ---")
inspect('EQOA_Frontiers.iso')
