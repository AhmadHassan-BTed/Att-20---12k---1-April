import struct

def parse_vif_code(code_32bit):
    imm = code_32bit & 0xFFFF
    num = (code_32bit >> 16) & 0xFF
    cmd = (code_32bit >> 24) & 0xFF
    return {'imm': imm, 'num': num, 'cmd': cmd}

def dump_w(data):
    pos = 0
    file_len = len(data)
    w_values = set()
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', data, pos)[0]
        vif = parse_vif_code(code)
        
        if vif['cmd'] == 0x6C and vif['num'] > 0:
            align_offset = (pos + 15) & ~15
            num = vif['num']
            for i in range(num):
                v_offset = align_offset + (i * 16)
                if v_offset + 16 <= file_len:
                    x, y, z, w = struct.unpack_from('<ffff', data, v_offset)
                    w_int = struct.unpack_from('<I', data, v_offset + 12)[0]
                    # Print first 10
                    if len(w_values) < 20:
                        print(f"Vertex {i}: X={x:.2f}, Y={y:.2f}, Z={z:.2f}, W(float)={w}, W(int)={w_int}, W(hex)=0x{w_int:X}")
                    w_values.add(w_int)
            pos = align_offset + (num * 16) - 4
        pos += 4
    print(f"\nUnique W integer values found: {sorted(list(w_values))[:50]}")

with open('workspace/payloads/asset_0x05AEBA67.bin', 'rb') as f:
    dump_w(f.read())
