import struct

def dump_header(filepath, label):
    print(f"\n=== ESF Header: {label} ({filepath}) ===")
    with open(filepath, 'rb') as f:
        hdr = f.read(32)
    magic = hdr[:4]
    version = struct.unpack_from('<I', hdr, 4)[0]
    constant = struct.unpack_from('<I', hdr, 8)[0]
    reserved1 = struct.unpack_from('<I', hdr, 12)[0]
    header_size = struct.unpack_from('<I', hdr, 16)[0]
    
    print(f"  Magic:       {magic}")
    print(f"  Version:     {version}")
    print(f"  Constant:    0x{constant:X} ({constant})")
    print(f"  Reserved1:   {reserved1}")
    print(f"  Header Size: {header_size}")
    print(f"  Raw Hex:     {hdr.hex()}")

dump_header('workspace/original/CHAR.ESF', "Vanilla CHAR.ESF")
dump_header('workspace/expansion/CHAR.ESF', "Frontiers CHAR.ESF")
dump_header('workspace/FINAL_CHAR_MERGED.ESF', "Patched Frontiers CHAR.ESF")
