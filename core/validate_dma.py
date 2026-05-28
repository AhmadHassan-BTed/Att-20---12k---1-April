import os
import sys
import struct
import glob
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def parse_dma_tag(tag_64bit):
    qwc = tag_64bit & 0xFFFF
    priority = (tag_64bit >> 26) & 0x3
    id_val = (tag_64bit >> 28) & 0x7
    irq = (tag_64bit >> 31) & 0x1
    addr = (tag_64bit >> 32) & 0xFFFFFFFF
    
    id_names = {0: 'refe', 1: 'cnt', 2: 'next', 3: 'ref', 4: 'refs', 5: 'call', 6: 'ret', 7: 'end'}
    return {
        'qwc': qwc,
        'id': id_val,
        'id_name': id_names.get(id_val, 'unknown'),
        'addr': addr
    }

def validate_dma_chain(data, start_offset=0):
    pos = start_offset
    file_len = len(data)
    chain_valid = True
    tags_found = 0
    
    pos = (pos + 15) & ~15
    
    while pos + 16 <= file_len:
        tag_64 = struct.unpack_from('<Q', data, pos)[0]
        vif_64 = struct.unpack_from('<Q', data, pos + 8)[0]
        
        dma = parse_dma_tag(tag_64)
        
        if dma['id_name'] == 'unknown' or (dma['qwc'] > 0x4000 and dma['id_name'] != 'ref'):
            print(f"    [!] Chain Break at 0x{pos:X}: Invalid Tag ID {dma['id']} or abnormal QWC ({dma['qwc']})")
            chain_valid = False
            break
            
        tags_found += 1
        
        if dma['id_name'] == 'end':
            break
            
        jump_bytes = dma['qwc'] * 16
        pos += 16 + jump_bytes

    if tags_found == 0:
        return False, "No tags found"
    elif chain_valid:
        return True, f"Valid ({tags_found} tags)"
    else:
        return False, "Broken chain"

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

def find_node(node, type_id):
    if node['type_id'] == type_id:
        return node
    for child in node['children']:
        res = find_node(child, type_id)
        if res: return res
    return None

def main():
    print("[*] Validating DMA Chains in Patched Payloads...")
    payloads = glob.glob("workspace/payloads/*.bin")
    
    for payload in payloads:
        with open(payload, 'rb') as f:
            data = f.read()
            
        root, _ = parse_node(data, 0)
        geom = find_node(root, 0x02610)
        if not geom:
            print(f"[-] {os.path.basename(payload)}: No 0x02610 node found.")
            continue
            
        # The DMA chain is usually in a leaf child of 0x02610
        valid = False
        def check_leaves(n):
            nonlocal valid
            if n['child_count'] == 0 and n['inline_data'] and len(n['inline_data']) > 16:
                # search for DMA tag signature
                # DMA tags often start at offset 0 or 16 in these buffers
                is_valid, msg = validate_dma_chain(n['inline_data'], 0)
                if is_valid:
                    valid = True
                else:
                    is_valid, msg = validate_dma_chain(n['inline_data'], 16)
                    if is_valid:
                        valid = True
            for c in n['children']:
                check_leaves(c)
                
        check_leaves(geom)
        
        if valid:
            print(f"[+] {os.path.basename(payload)}: DMA Chain OK")
        else:
            print(f"[-] {os.path.basename(payload)}: DMA Chain BROKEN")

if __name__ == "__main__":
    main()
