#!/usr/bin/env python3
"""
Bone Node Comparison Tool (v2)
================================
Compares Child 6 bone nodes between Frontiers and Vanilla EQOA character models.
Extracts the first bone node from each ESF file and performs detailed comparison.
"""

import os
import sys
import struct

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'core'))
from esf_parser import ESFParser


def find_first_bone_node(esf_data):
    """Parse ESF and find the first bone node (type 0x22400 or 0x12400)."""
    try:
        parser = ESFParser(esf_data)
        parser.parse()
    except Exception as e:
        print(f"[-] Parse failed: {e}")
        return None

    # Search for first bone node
    def search_tree(node, depth=0):
        if node['type_id'] in [0x22400, 0x12400]:
            return node

        for child in node['children']:
            result = search_tree(child, depth + 1)
            if result:
                return result

        return None

    bone_node = search_tree(parser.root)
    return {
        'parser': parser,
        'node': bone_node
    }


def print_hex_dump(data, label, start_offset=0, max_bytes=512):
    """Print hex dump of binary data."""
    print(f"\n{label}")
    print("=" * 100)

    bytes_to_show = min(len(data), max_bytes)

    for offset in range(0, bytes_to_show, 16):
        chunk = data[offset:offset + 16]
        hex_str = ' '.join(f'{b:02x}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        actual_offset = start_offset + offset
        print(f"  0x{actual_offset:08x}: {hex_str:<48} | {ascii_str}")

    if len(data) > max_bytes:
        print(f"  ... ({len(data) - max_bytes:,} more bytes)")
    print("=" * 100)


def main():
    esf_files = [
        ('workspace/expansion/CHAR.ESF', 'FRONTIERS', 0x22400),
        ('workspace/original/CHAR.ESF', 'VANILLA', 0x12400),
    ]

    print("=" * 100)
    print("BONE NODE COMPARISON: FRONTIERS vs VANILLA")
    print("=" * 100)

    bone_data = {}

    for esf_path, label, expected_type in esf_files:
        print(f"\n[*] Processing {label}: {esf_path}")

        if not os.path.exists(esf_path):
            print(f"[-] File not found")
            continue

        with open(esf_path, 'rb') as f:
            esf_data = f.read()
        print(f"    ESF Size: {len(esf_data):,} bytes")

        result = find_first_bone_node(esf_data)
        if not result or not result['node']:
            print(f"[-] Could not find bone node")
            continue

        node = result['node']
        parser = result['parser']

        print(f"[+] Found bone node:")
        print(f"    Type ID: 0x{node['type_id']:05X} (expected: 0x{expected_type:05X})")
        print(f"    Data Size: {node['data_size']:,} bytes")
        print(f"    Child Count: {node['child_count']}")
        print(f"    Offset in ESF: 0x{node['offset']:X}")

        # Extract node binary (header + data)
        node_offset = node['offset']
        header_size = 12
        data_size = node['data_size']
        total_size = header_size + data_size

        # Verify bounds
        if node_offset + total_size > len(esf_data):
            print(f"[-] Node extends beyond file bounds!")
            continue

        node_binary = esf_data[node_offset:node_offset + total_size]

        bone_data[label] = {
            'type_id': node['type_id'],
            'data_size': node['data_size'],
            'child_count': node['child_count'],
            'offset': node_offset,
            'total_size': total_size,
            'binary_data': node_binary,
            'esf_data': esf_data
        }

    if len(bone_data) < 2:
        print("\n[-] Could not extract bone data from both ESF files")
        sys.exit(1)

    frontiers = bone_data['FRONTIERS']
    vanilla = bone_data['VANILLA']

    # Comparison Report
    print("\n" + "=" * 100)
    print("COMPARISON REPORT")
    print("=" * 100)

    print(f"\n[*] TYPE ID COMPARISON:")
    print(f"    Frontiers: 0x{frontiers['type_id']:05X}")
    print(f"    Vanilla:   0x{vanilla['type_id']:05X}")
    print(f"    Match:     {frontiers['type_id'] == vanilla['type_id']}")

    if frontiers['type_id'] != vanilla['type_id']:
        print(f"\n    [!] TYPE IDs DIFFER - Conversion required!")
        print(f"        Must convert type 0x{vanilla['type_id']:05X} -> 0x{frontiers['type_id']:05X}")

    print(f"\n[*] DATA SIZE COMPARISON:")
    print(f"    Frontiers: {frontiers['data_size']:,} bytes")
    print(f"    Vanilla:   {vanilla['data_size']:,} bytes")
    print(f"    Match:     {frontiers['data_size'] == vanilla['data_size']}")

    if frontiers['data_size'] != vanilla['data_size']:
        print(f"    [!] SIZE DIFFERS - Data structure may differ")
        print(f"        Frontiers is {frontiers['data_size'] - vanilla['data_size']:+,} bytes")

    print(f"\n[*] CHILD COUNT COMPARISON:")
    print(f"    Frontiers: {frontiers['child_count']}")
    print(f"    Vanilla:   {vanilla['child_count']}")
    print(f"    Match:     {frontiers['child_count'] == vanilla['child_count']}")

    # Binary comparison
    print(f"\n[*] BINARY DATA COMPARISON:")

    min_len = min(len(frontiers['binary_data']), len(vanilla['binary_data']))
    differences = 0
    first_diff_offset = None
    diff_map = []

    for i in range(min_len):
        if frontiers['binary_data'][i] != vanilla['binary_data'][i]:
            if first_diff_offset is None:
                first_diff_offset = i
            differences += 1
            if len(diff_map) < 10:  # Record first 10 differences
                diff_map.append((i, frontiers['binary_data'][i], vanilla['binary_data'][i]))

    total_compared = min_len
    pct_diff = (100 * differences / min_len) if min_len > 0 else 0

    print(f"    Total bytes compared: {total_compared:,}")
    print(f"    Bytes that differ: {differences:,} ({pct_diff:.2f}%)")

    if differences == 0:
        print(f"    [+] Binary data is IDENTICAL - No conversion needed")
    elif differences == total_compared:
        print(f"    [-] Binary data is COMPLETELY DIFFERENT - Full conversion needed")
    else:
        print(f"    [!] Binary data is PARTIALLY DIFFERENT")
        if first_diff_offset is not None:
            print(f"        First difference at offset: 0x{first_diff_offset:X} (byte offset in combined data)")
            print(f"        First 10 differences:")
            for offset, f_byte, v_byte in diff_map:
                print(f"          0x{offset:X}: Frontiers=0x{f_byte:02X}, Vanilla=0x{v_byte:02X}")

    if len(frontiers['binary_data']) != len(vanilla['binary_data']):
        print(f"\n    Size mismatch in total binary data:")
        print(f"      Frontiers: {len(frontiers['binary_data']):,} bytes")
        print(f"      Vanilla:   {len(vanilla['binary_data']):,} bytes")
        print(f"      Difference: {abs(len(frontiers['binary_data']) - len(vanilla['binary_data'])):+,} bytes")

    # Hex dumps
    print_hex_dump(
        frontiers['binary_data'],
        "\n[*] FRONTIERS - First 512 bytes (Type 0x{:05X}, Data Size: {:,})".format(
            frontiers['type_id'], frontiers['data_size']
        ),
        start_offset=frontiers['offset']
    )

    print_hex_dump(
        vanilla['binary_data'],
        "\n[*] VANILLA - First 512 bytes (Type 0x{:05X}, Data Size: {:,})".format(
            vanilla['type_id'], vanilla['data_size']
        ),
        start_offset=vanilla['offset']
    )

    # Analysis
    print("\n" + "=" * 100)
    print("ANALYSIS & RECOMMENDATIONS")
    print("=" * 100)

    print("\n[*] FINDINGS:")

    if frontiers['type_id'] != vanilla['type_id']:
        print(f"    1. Type ID differs: 0x{vanilla['type_id']:05X} vs 0x{frontiers['type_id']:05X}")
        print(f"       Action: Type ID must be changed during conversion")
    else:
        print(f"    1. Type ID is consistent: 0x{vanilla['type_id']:05X}")
        print(f"       Action: No type ID change needed")

    if frontiers['data_size'] != vanilla['data_size']:
        print(f"    2. Data size differs: {vanilla['data_size']:,} vs {frontiers['data_size']:,}")
        print(f"       Difference: {frontiers['data_size'] - vanilla['data_size']:+,} bytes")
        if abs(frontiers['data_size'] - vanilla['data_size']) > 100:
            print(f"       Action: Likely needs data structure expansion/reorganization")
        else:
            print(f"       Action: Might be padding or minor fields")
    else:
        print(f"    2. Data size is consistent: {vanilla['data_size']:,} bytes")
        print(f"       Action: No size adjustment needed")

    if differences > 0:
        print(f"    3. Binary content differs: {pct_diff:.1f}% mismatch")
        if pct_diff > 50:
            print(f"       Action: Full data structure conversion required")
        else:
            print(f"       Action: Targeted field mapping needed")
    else:
        print(f"    3. Binary content is identical")
        print(f"       Action: Type ID change only (if needed)")

    print("\n[*] CONCLUSION:")
    if frontiers['type_id'] == vanilla['type_id'] and differences == 0:
        print("    No data structure conversion required.")
        print("    Only type ID change needed (if upgrading Vanilla to Frontiers format).")
    elif frontiers['type_id'] != vanilla['type_id'] and differences == 0:
        print("    Minimal conversion: Only type ID needs to be changed.")
        print("    Binary data layout is compatible.")
    else:
        print("    Data structure conversion IS REQUIRED beyond type ID change.")
        print("    Binary data differs, indicating structural incompatibility.")
        if frontiers['data_size'] != vanilla['data_size']:
            print("    Size difference suggests content expansion or reorganization.")

    print("\n" + "=" * 100)


if __name__ == '__main__':
    main()
