import os
import sys
import math
import struct
import glob
import hashlib
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def parse_node(d, p):
    if p + 12 > len(d): return None, p
    tid = struct.unpack_from('<I', d, p)[0]
    sz  = struct.unpack_from('<I', d, p + 4)[0]
    cnt = struct.unpack_from('<I', d, p + 8)[0]
    node = {'type_id': tid, 'data_size': sz, 'child_count': cnt, 'children': [], 'inline_data': None}
    p += 12
    if cnt == 0:
        node['inline_data'] = d[p:p+sz]
        p += sz
    else:
        for _ in range(cnt):
            child, p = parse_node(d, p)
            if child: node['children'].append(child)
    return node, p

def serialize_node(node):
    buf = bytearray()
    buf += struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
    if node['child_count'] == 0:
        if node['inline_data']:
            buf += node['inline_data']
    else:
        for child in node['children']:
            buf += serialize_node(child)
    return bytes(buf)

def sanitize_buffer(data):
    fixed_nans = 0
    fixed_weights = 0
    fixed_indices = 0
    
    pos = 0
    file_len = len(data)
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', data, pos)[0]
        cmd = (code >> 24) & 0xFF
        num = (code >> 16) & 0xFF
        
        if cmd == 0x6C and num > 0:  # V4-32
            v_off = pos + 4
            expected = num * 16
            if v_off + expected <= file_len:
                for i in range(num):
                    f = list(struct.unpack_from('<ffff', data, v_off))
                    i_vals = list(struct.unpack_from('<IIII', data, v_off))
                    
                    # 1. NaN / Inf Check
                    for j in range(4):
                        if math.isnan(f[j]) or math.isinf(f[j]):
                            f[j] = 0.0
                            i_vals[j] = struct.unpack('<I', struct.pack('<f', 0.0))[0]
                            fixed_nans += 1
                            
                    # 2. Index Clamping (W component is often Bone Index)
                    if 0 < i_vals[3] < 1000:
                        b_idx = i_vals[3] // 4 if i_vals[3] % 4 == 0 else i_vals[3]
                        if b_idx > 42:
                            i_vals[3] = 0
                            fixed_indices += 1
                            
                    # 3. Weight Normalization
                    # If the 4 floats are all between 0 and 1.01, it's highly likely a weight vector
                    if all(0.0 <= x <= 1.01 for x in f) and 0.01 < sum(f) < 2.0:
                        w_sum = sum(f)
                        if abs(w_sum - 1.0) > 0.001:
                            for j in range(4):
                                f[j] /= w_sum
                                i_vals[j] = struct.unpack('<I', struct.pack('<f', f[j]))[0]
                            fixed_weights += 1
                            
                    struct.pack_into('<IIII', data, v_off, *i_vals)
                    v_off += 16
                pos = (pos + 4 + expected + 3) & ~3
            else:
                pos += 4
                
        elif cmd == 0x6D and num > 0: # V4-16
            v_off = pos + 4
            expected = num * 8
            if v_off + expected <= file_len:
                for i in range(num):
                    s = list(struct.unpack_from('<HHHH', data, v_off))
                    # Check W component for bone index
                    if 42 < s[3] < 255:
                        s[3] = 0
                        fixed_indices += 1
                    struct.pack_into('<HHHH', data, v_off, *s)
                    v_off += 8
                pos = (pos + 4 + expected + 3) & ~3
            else:
                pos += 4
        else:
            pos += 4
            
    return fixed_nans, fixed_weights, fixed_indices

def main():
    print("=" * 80)
    print("  PS2 GEOMETRY SANITIZER & VIF NORMALIZER")
    print("=" * 80)
    
    payloads = glob.glob("workspace/payloads/*.bin")
    if not payloads:
        print("[-] No payloads found.")
        sys.exit(1)
        
    total_nans = 0
    total_weights = 0
    total_indices = 0
    
    for payload in payloads:
        with open(payload, 'rb') as f:
            data = bytearray(f.read())
            
        root, _ = parse_node(data, 0)
        
        def scrub_tree(node):
            nonlocal total_nans, total_weights, total_indices
            if node['child_count'] == 0 and node['inline_data']:
                if len(node['inline_data']) > 16:
                    buf = bytearray(node['inline_data'])
                    n, w, i = sanitize_buffer(buf)
                    node['inline_data'] = bytes(buf)
                    total_nans += n
                    total_weights += w
                    total_indices += i
            for child in node['children']:
                scrub_tree(child)
                
        scrub_tree(root)
        
        new_data = serialize_node(root)
        with open(payload, 'wb') as f:
            f.write(new_data)
            
    print("\n[*] Pre-Normalization vs Post-Normalization Stats:")
    print(f"    -> Found {total_nans} vertices with NaN/Inf corrupted floats. Fixed {total_nans}.")
    print(f"    -> Found {total_weights} vertices with non-normalized weights. Fixed {total_weights}.")
    print(f"    -> Found {total_indices} vertices referencing Bones > 42. Clamped to Root Bone (0).")
    
    print("\n[*] Rebuilding FINAL_CHAR_MERGED.ESF with Sanitized Geometry...")
    import subprocess
    subprocess.run([sys.executable, "-m", "core.esf_rebuilder"], check=True)
    
    sha256 = hashlib.sha256()
    with open("workspace/FINAL_CHAR_MERGED.ESF", 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b""):
            sha256.update(chunk)
            
    print(f"\n[+] Sanitization Complete!")
    print(f"    Sanitized Buffer Hex Signature (SHA-256): {sha256.hexdigest()}")

if __name__ == '__main__':
    main()
