import struct

def analyze_skeleton(filepath):
    print(f"\n=== Analyzing Skeleton in {filepath} ===")
    with open(filepath, 'rb') as f:
        data = f.read()
    
    # We find the 0x12400 or 0x22400 skeleton node
    # Since these payload files are single model node subtrees, let's find the skeleton node
    type_id = struct.unpack_from('<I', data, 0)[0]
    data_size = struct.unpack_from('<I', data, 4)[0]
    child_count = struct.unpack_from('<I', data, 8)[0]
    
    # We recursively walk the tree to find skeleton node
    skel_data = None
    skel_type = None
    
    def walk(pos):
        nonlocal skel_data, skel_type
        if pos >= len(data):
            return pos
        tid = struct.unpack_from('<I', data, pos)[0]
        dsz = struct.unpack_from('<I', data, pos + 4)[0]
        cc = struct.unpack_from('<I', data, pos + 8)[0]
        
        next_pos = pos + 12
        if cc == 0:
            if tid in (0x12400, 0x22400):
                skel_data = data[next_pos : next_pos + dsz]
                skel_type = tid
            next_pos += dsz
        else:
            for _ in range(cc):
                next_pos = walk(next_pos)
        return next_pos

    walk(0)
    
    if skel_data is None:
        print("[-] Skeleton node not found!")
        return
        
    print(f"Skeleton type: 0x{skel_type:05X}, size: {len(skel_data)} bytes")
    # First 16 bytes: 4 floats
    header = struct.unpack('<4f', skel_data[:16])
    print(f"Header Floats: {header}")
    
    # Let's inspect the remaining bytes as records
    records_data = skel_data[16:]
    # Each record starts with an Int (bone ID) and Int (parent ID)
    # Let's find the offsets where parent ID is -1 or matches parental hierarchy, to determine record size!
    # A parent ID is usually between -1 and 100.
    # Let's print the first few integers
    for i in range(0, min(len(records_data), 300), 4):
        val = struct.unpack_from('<i', records_data, i)[0]
        fval = struct.unpack_from('<f', records_data, i)[0]
        print(f"  Offset {i:03X} (+16={i+16:03X}): Int={val:11d} | Float={fval:>12.5f}")

analyze_skeleton('workspace/payloads/asset_0x2EF8E480.bin')
analyze_skeleton('workspace/frontiers_reference.bin')
