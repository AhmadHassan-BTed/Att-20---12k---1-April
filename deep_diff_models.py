import os, struct

def parse_node(data, pos):
    if pos + 12 > len(data):
        return None, pos
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node = {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'offset': pos,
        'children': [],
        'inline_data': None
    }
    
    next_pos = pos + 12
    if child_count == 0:
        node['inline_data'] = data[next_pos:next_pos+data_size]
        next_pos += data_size
    else:
        for _ in range(child_count):
            child, next_pos = parse_node(data, next_pos)
            if child is not None:
                node['children'].append(child)
            
    return node, next_pos

def build_tree_flat(node, path=""):
    flat = {}
    nodepath = f"{path}/{node['type_id']:05X}" if path else f"{node['type_id']:05X}"
    flat[nodepath] = node
    
    if node['child_count'] > 0:
        type_counters = {}
        for child in node['children']:
            ctid = child['type_id']
            type_counters[ctid] = type_counters.get(ctid, 0) + 1
            suffix = f"_{type_counters[ctid]}"
            child_path = f"{nodepath}/{ctid:05X}{suffix}"
            child_flat = build_tree_flat(child, nodepath)
            for k, v in child_flat.items():
                key_parts = k.split('/')
                key_parts[len(nodepath.split('/'))] += suffix
                unique_key = '/'.join(key_parts)
                flat[unique_key] = v
    return flat

with open('workspace/payloads/asset_0x2EF8E480.bin', 'rb') as f:
    van_data = f.read()
van_node, _ = parse_node(van_data, 0)

with open('workspace/frontiers_reference.bin', 'rb') as f:
    fro_data = f.read()
fro_node, _ = parse_node(fro_data, 0)

van_flat = build_tree_flat(van_node)
fro_flat = build_tree_flat(fro_node)

# Strip the root type prefix (e.g. "62700/" or "72700/") from paths to align them
def strip_root(flat_dict):
    new_dict = {}
    for k, v in flat_dict.items():
        parts = k.split('/')
        new_key = '/'.join(parts[1:]) if len(parts) > 1 else "ROOT"
        new_dict[new_key] = v
    return new_dict

van_stripped = strip_root(van_flat)
fro_stripped = strip_root(fro_flat)

print(f"Aligned Vanilla nodes:    {len(van_stripped)}")
print(f"Aligned Frontiers nodes:  {len(fro_stripped)}")

print("\n=== ONE-TO-ONE STRUCTURE CORRESPONDENCE ===")
all_paths = sorted(list(set(van_stripped.keys()) | set(fro_stripped.keys())))

print(f"{'Aligned Node Path':<50} | {'Vanilla Size':<12} | {'Frontiers Size':<12} | Status")
print("-" * 90)

mismatches = 0
for p in all_paths:
    v_node = van_stripped.get(p)
    f_node = fro_stripped.get(p)
    
    if v_node and f_node:
        v_size = len(v_node['inline_data']) if v_node['child_count'] == 0 else v_node['data_size']
        f_size = len(f_node['inline_data']) if f_node['child_count'] == 0 else f_node['data_size']
        status = "MATCH" if v_size == f_size else f"DIFF (van={v_size:,}, fro={f_size:,})"
        if v_size != f_size:
            mismatches += 1
            print(f"{p:<50} | {v_size:>12,} | {f_size:>12,} | {status}")
    elif v_node:
        v_size = len(v_node['inline_data']) if v_node['child_count'] == 0 else v_node['data_size']
        print(f"{p:<50} | {v_size:>12,} | {'MISSING':<12} | VANILLA ONLY")
        mismatches += 1
    else:
        f_size = len(f_node['inline_data']) if f_node['child_count'] == 0 else f_node['data_size']
        print(f"{p:<50} | {'MISSING':<12} | {f_size:>12,} | FRONTIERS ONLY")
        mismatches += 1

print(f"\nTotal structural discrepancies: {mismatches}")
