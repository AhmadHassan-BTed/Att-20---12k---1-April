import os
import struct

def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
    }
    pos += 12
    if child_count == 0:
        node['inline_data'] = data[pos : pos + data_size]
        pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def main():
    payloads_dir = 'workspace/payloads'
    if not os.path.exists(payloads_dir):
        print(f"[-] Payloads directory {payloads_dir} does not exist.")
        return
        
    bins = [f for f in os.listdir(payloads_dir) if f.endswith('.bin')]
    if not bins:
        print("[-] No .bin files in payloads directory.")
        return
        
    print(f"[+] Verifying {len(bins)} payloads in {payloads_dir}...")
    for b in bins:
        path = os.path.join(payloads_dir, b)
        with open(path, 'rb') as f:
            data = f.read()
        
        node, end_pos = parse_node(data, 0)
        print(f"File {b}: size={len(data):,}, root_type=0x{node['type_id']:X}, children count={len(node['children'])}")
        if len(node['children']) != 17:
            print(f"  [!] WARNING: child count is {len(node['children'])}, expected 17!")
        else:
            print(f"  [+] PASS: 17 children.")

if __name__ == '__main__':
    main()
