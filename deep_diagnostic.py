#!/usr/bin/env python3
"""
EQOA Deep Binary Diagnostic
============================
Determines exactly WHY the injected models are invisible by verifying
the parse->serialize round-trip integrity and checking the final ESF.
"""
import os, sys, struct, json, hashlib

def parse_node_raw(data, pos, depth=0):
    """Parse a node and return (node, end_pos, bytes_consumed_by_children)."""
    if pos + 12 > len(data):
        return None, pos
    
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    node_start = pos
    next_pos = pos + 12
    
    if child_count == 0:
        next_pos += data_size
        children_actual_size = 0
    else:
        children_start = next_pos
        for _ in range(child_count):
            _, next_pos = parse_node_raw(data, next_pos, depth + 1)
        children_actual_size = next_pos - children_start
    
    return {
        'type_id': type_id,
        'data_size': data_size,
        'child_count': child_count,
        'offset': node_start,
        'children_actual_size': children_actual_size,
        'trailing_bytes': data_size - children_actual_size if child_count > 0 else 0,
    }, next_pos


def diagnose_round_trip(filepath, label):
    """Check if parse->serialize loses any bytes."""
    from esf_parser import ESFParser
    from esf_rebuilder import serialize_node
    
    with open(filepath, 'rb') as f:
        original = f.read()
    
    # Parse through ESFParser
    parser = ESFParser(original)
    node, end_pos = parser._parse_node(0)
    
    # Serialize back
    serialized = serialize_node(node)
    
    print(f"\n{'='*60}")
    print(f"ROUND-TRIP DIAGNOSIS: {label}")
    print(f"{'='*60}")
    print(f"  Original size:   {len(original):>10,} bytes")
    print(f"  Serialized size: {len(serialized):>10,} bytes")
    print(f"  Parse end pos:   0x{end_pos:X} ({end_pos:,})")
    print(f"  Delta:           {len(serialized) - len(original):>+10,} bytes")
    
    if original == serialized:
        print(f"  [PASS] Perfect round-trip! No bytes lost.")
        return True
    else:
        # Find first difference
        min_len = min(len(original), len(serialized))
        first_diff = None
        diff_count = 0
        for i in range(min_len):
            if original[i] != serialized[i]:
                if first_diff is None:
                    first_diff = i
                diff_count += 1
        
        if first_diff is not None:
            print(f"  [FAIL] First byte difference at offset 0x{first_diff:X}")
            print(f"  [FAIL] Total differing bytes: {diff_count:,}")
            print(f"         Original[0x{first_diff:X}]: 0x{original[first_diff]:02X}")
            print(f"         Serialized[0x{first_diff:X}]: 0x{serialized[first_diff]:02X}")
            
            # Show context around first difference
            ctx_start = max(0, first_diff - 16)
            ctx_end = min(min_len, first_diff + 16)
            print(f"\n  Context (offset 0x{ctx_start:X} to 0x{ctx_end:X}):")
            print(f"    Original:   {original[ctx_start:ctx_end].hex()}")
            print(f"    Serialized: {serialized[ctx_start:ctx_end].hex()}")
        
        if len(original) != len(serialized):
            print(f"\n  [FAIL] Size mismatch! Lost {len(original) - len(serialized):,} bytes")
        
        return False


def diagnose_trailing_bytes(filepath, label):
    """Check for trailing bytes in branch nodes."""
    with open(filepath, 'rb') as f:
        data = f.read()
    
    print(f"\n{'='*60}")
    print(f"TRAILING BYTES ANALYSIS: {label}")
    print(f"{'='*60}")
    
    node, end_pos = parse_node_raw(data, 0)
    
    total_trailing = node['trailing_bytes']
    print(f"  Root node: type=0x{node['type_id']:05X}, "
          f"data_size={node['data_size']:,}, "
          f"children_actual={node['children_actual_size']:,}, "
          f"trailing={node['trailing_bytes']:,}")
    
    if node['trailing_bytes'] > 0:
        trailing_start = 12 + node['children_actual_size']
        trailing_end = 12 + node['data_size']
        print(f"  Trailing data at [0x{trailing_start:X}:0x{trailing_end:X}]:")
        print(f"    Hex: {data[trailing_start:trailing_end].hex()}")
    
    # File-level trailing
    file_trailing = len(data) - end_pos
    if file_trailing > 0:
        print(f"  File-level trailing: {file_trailing} bytes after parsed tree")
        print(f"    Hex: {data[end_pos:].hex()}")
    
    return total_trailing


def diagnose_final_esf(esf_path, json_path):
    """Verify the injected models in the final ESF."""
    print(f"\n{'='*60}")
    print(f"FINAL ESF INJECTION VERIFICATION")
    print(f"{'='*60}")
    
    if not os.path.exists(esf_path):
        print(f"  [-] {esf_path} not found!")
        return
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
    
    with open(esf_path, 'rb') as f:
        esf_data = f.read()
    
    print(f"  ESF size: {len(esf_data):,} bytes")
    print(f"  SHA-256: {hashlib.sha256(esf_data).hexdigest()}")
    
    # Parse the ESF
    from esf_parser import ESFParser
    parser = ESFParser(esf_data).parse()
    
    print(f"  Root children: {parser.root['child_count']}")
    print(f"  Pointer table entries: {len(parser.pointer_table)}")
    
    # Look up each target model
    pt_map = {e.asset_id: e for e in parser.pointer_table}
    
    for t in targets:
        h = int(t['expansion_hash'], 16)
        if h in pt_map:
            entry = pt_map[h]
            # Read the node header at this offset
            node_type = struct.unpack_from('<I', esf_data, entry.offset)[0]
            node_dsize = struct.unpack_from('<I', esf_data, entry.offset + 4)[0]
            node_cc = struct.unpack_from('<I', esf_data, entry.offset + 8)[0]
            
            print(f"\n  Model {t['expansion_hash']}:")
            print(f"    Offset: 0x{entry.offset:X}, Length: {entry.length:,}")
            print(f"    type_id: 0x{node_type:05X}, data_size: {node_dsize:,}, children: {node_cc}")
            print(f"    Expected type: 0x72700")
            
            if node_type == 0x72700:
                print(f"    [OK] Type matches Frontiers format")
            elif node_type == 0x62700:
                print(f"    [FAIL] Still has Vanilla type!")
            else:
                print(f"    [WARN] Unexpected type: 0x{node_type:05X}")
            
            # Check if the injected data matches what we intended
            payload_path = f"workspace/payloads/asset_{t['expansion_hash']}.bin"
            if os.path.exists(payload_path):
                with open(payload_path, 'rb') as f:
                    payload_data = f.read()
                
                injected_data = esf_data[entry.offset:entry.offset + len(payload_data)]
                
                if injected_data == payload_data:
                    print(f"    [PASS] Injected bytes match payload file perfectly")
                else:
                    # Find differences
                    diff_count = 0
                    first_diff = None
                    for i in range(min(len(injected_data), len(payload_data))):
                        if injected_data[i] != payload_data[i]:
                            if first_diff is None:
                                first_diff = i
                            diff_count += 1
                    print(f"    [FAIL] {diff_count:,} byte differences!")
                    if first_diff is not None:
                        print(f"           First diff at offset 0x{first_diff:X}")
                    if len(injected_data) != len(payload_data):
                        print(f"           Size mismatch: ESF has {len(injected_data):,}, "
                              f"payload has {len(payload_data):,}")
        else:
            print(f"\n  Model {t['expansion_hash']}: NOT FOUND in pointer table!")


def diagnose_original_vs_expansion(json_path):
    """Compare a vanilla model against its Frontiers counterpart at the binary level."""
    print(f"\n{'='*60}")
    print(f"VANILLA vs FRONTIERS STRUCTURAL COMPARISON")
    print(f"{'='*60}")
    
    with open(json_path, 'r') as f:
        targets = json.load(f)
    
    with open('workspace/expansion/CHAR.ESF', 'rb') as f:
        exp_data = f.read()
    
    # Just do the first model for detailed analysis
    t = targets[0]
    
    # Read vanilla payload
    payload_path = f"workspace/payloads/asset_{t['original_hash']}.bin"
    with open(payload_path, 'rb') as f:
        van_data = f.read()
    
    # Read frontiers model from ESF
    fro_data = exp_data[t['expansion_offset']:t['expansion_offset'] + t['expansion_length']]
    
    print(f"\n  Model: {t['original_hash']}")
    print(f"  Vanilla size:   {len(van_data):,}")
    print(f"  Frontiers size: {len(fro_data):,}")
    
    # Parse both trees fully
    van_node, van_end = parse_node_raw(van_data, 0)
    fro_node, fro_end = parse_node_raw(fro_data, 0)
    
    print(f"\n  Vanilla  tree: type=0x{van_node['type_id']:05X} children={van_node['child_count']} "
          f"data_size={van_node['data_size']:,} trailing={van_node['trailing_bytes']}")
    print(f"  Frontiers tree: type=0x{fro_node['type_id']:05X} children={fro_node['child_count']} "
          f"data_size={fro_node['data_size']:,} trailing={fro_node['trailing_bytes']}")
    print(f"  Vanilla  file trailing: {len(van_data) - van_end}")
    print(f"  Frontiers file trailing: {len(fro_data) - fro_end}")


def main():
    payload_dir = 'workspace/payloads'
    json_path = 'workspace/target_assets.json'
    final_esf = 'workspace/FINAL_CHAR_MERGED.ESF'
    
    # Test 1: Round-trip integrity for a vanilla payload
    first_payload = os.path.join(payload_dir, 'asset_0x05AEBA67.bin')
    if os.path.exists(first_payload):
        diagnose_round_trip(first_payload, "Vanilla Payload 0x05AEBA67")
        diagnose_trailing_bytes(first_payload, "Vanilla Payload 0x05AEBA67")
    
    # Test 2: Round-trip integrity for a Frontiers model
    # Extract a frontiers model for comparison
    with open(json_path, 'r') as f:
        targets = json.load(f)
    t = targets[0]
    with open('workspace/expansion/CHAR.ESF', 'rb') as f:
        f.seek(t['expansion_offset'])
        fro_model = f.read(t['expansion_length'])
    fro_path = 'workspace/frontiers_reference.bin'
    with open(fro_path, 'wb') as f:
        f.write(fro_model)
    diagnose_round_trip(fro_path, "Frontiers Reference 0x2EF8E480")
    diagnose_trailing_bytes(fro_path, "Frontiers Reference 0x2EF8E480")
    
    # Test 3: Structural comparison
    diagnose_original_vs_expansion(json_path)
    
    # Test 4: Final ESF verification
    diagnose_final_esf(final_esf, json_path)
    
    print(f"\n{'='*60}")
    print(f"DIAGNOSTIC COMPLETE")
    print(f"{'='*60}")


if __name__ == '__main__':
    main()
