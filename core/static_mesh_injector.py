import struct
import os

def force_static_weights(payload_data):
    """
    Scans the binary payload for 0x6C V4-32 VIF unpack arrays and overwrites
    the W-components with 0.0 (Identity weight, bound to Joint 0).
    """
    patched = bytearray(payload_data)
    pos = 12
    file_len = len(patched)
    count = 0
    
    while pos + 4 <= file_len:
        code = struct.unpack_from('<I', patched, pos)[0]
        cmd = (code >> 24) & 0xFF
        num = (code >> 16) & 0xFF
        addr = code & 0x7FFF
        
        if cmd == 0x6C and num > 0:
            v_offset = pos + 4
            expected_len = num * 16
            
            # Strict bounds validation to prevent false positives in raw float data
            if v_offset + expected_len <= file_len:
                for i in range(num):
                    struct.pack_into('<I', patched, v_offset + 12, 0x00000000)
                    v_offset += 16
                    count += 1
                pos = (pos + 4 + expected_len + 3) & ~3
            else:
                # False positive VIF tag, skip 4 bytes
                pos += 4
        else:
            pos += 4
            
    return bytes(patched), count

def process_static_injection(payload_path):
    if not os.path.exists(payload_path):
        return
        
    with open(payload_path, 'rb') as f:
        data = f.read()
        
    # We must only scan the actual VIF payload, but since it's an ESF tree,
    # the 0x6C signature could theoretically collide with node sizes.
    # However, since we are scrubbing ALL geometry to 0.0, we will use a safe 
    # tree parser to only target the leaf nodes inside the 0x02610 mesh block.
    
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
        
    root, _ = parse_node(data, 0)
    total_scrubbed = 0
    
    def scrub_tree(node):
        nonlocal total_scrubbed
        if node['child_count'] == 0 and node['inline_data']:
            if len(node['inline_data']) > 16:
                new_data, scrubbed = force_static_weights(node['inline_data'])
                node['inline_data'] = new_data
                total_scrubbed += scrubbed
        for child in node['children']:
            scrub_tree(child)
            
    scrub_tree(root)
    
    if total_scrubbed > 0:
        with open(payload_path, 'wb') as f:
            f.write(serialize_node(root))
        print(f"[*] Static Mesh Pinning: Scrubbed {total_scrubbed} vertices to Identity Matrix (Root Bone 0).")
    else:
        print("[-] Warning: No V4-32 vertices found to scrub.")

if __name__ == '__main__':
    # Test script
    import sys
    if len(sys.argv) > 1:
        process_static_injection(sys.argv[1])
