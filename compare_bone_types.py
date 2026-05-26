#!/usr/bin/env python3
"""
Bone Node Comparison Tool (v3)
================================
Finds first 0x22400 (Frontiers) vs first 0x12400 (Vanilla) bone nodes and compares them.
"""

import os
import sys
import struct

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'core'))
from esf_parser import ESFParser


def find_bone_node_by_type(esf_data, target_type):
    """Parse ESF and find the first bone node of the specified type."""
    try:
        parser = ESFParser(esf_data)
        parser.parse()
    except Exception as e:
        print(f"[-] Parse failed: {e}")
        return None

    # Search for first bone node of target type
    def search_tree(node, depth=0):
        if node['type_id'] == target_type:
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
    print("=" * 100)
    print("BONE NODE TYPE COMPARISON: 0x22400 (FRONTIERS) vs 0x12400 (VANILLA)")
    print("=" * 100)

    # Load Frontiers ESF
    print(f"\n[*] Loading Frontiers ESF...")
    if not os.path.exists('workspace/expansion/CHAR.ESF'):
        print(f"[-] File not found")
        sys.exit(1)

    with open('workspace/expansion/CHAR.ESF', 'rb') as f:
        esf_frontiers = f.read()
    print(f"    Size: {len(esf_frontiers):,} bytes")

    # Load Vanilla ESF
    print(f"\n[*] Loading Vanilla ESF...")
    if not os.path.exists('workspace/original/CHAR.ESF'):
        print(f"[-] File not found")
        sys.exit(1)

    with open('workspace/original/CHAR.ESF', 'rb') as f:
        esf_vanilla = f.read()
    print(f"    Size: {len(esf_vanilla):,} bytes")

    # Find 0x22400 in Frontiers
    print(f"\n[*] Searching for 0x22400 in Frontiers...")
    result_22400 = find_bone_node_by_type(esf_frontiers, 0x22400)
    if not result_22400 or not result_22400['node']:
        print(f"[-] Could not find 0x22400 in Frontiers")
        sys.exit(1)

    node_22400 = result_22400['node']
    print(f"[+] Found 0x22400 node:")
    print(f"    Type ID: 0x{node_22400['type_id']:05X}")
    print(f"    Data Size: {node_22400['data_size']:,} bytes")
    print(f"    Child Count: {node_22400['child_count']}")
    print(f"    Offset: 0x{node_22400['offset']:X}")

    # Find 0x12400 in Vanilla
    print(f"\n[*] Searching for 0x12400 in Vanilla...")
    result_12400 = find_bone_node_by_type(esf_vanilla, 0x12400)
    if not result_12400 or not result_12400['node']:
        print(f"[-] Could not find 0x12400 in Vanilla")
        sys.exit(1)

    node_12400 = result_12400['node']
    print(f"[+] Found 0x12400 node:")
    print(f"    Type ID: 0x{node_12400['type_id']:05X}")
    print(f"    Data Size: {node_12400['data_size']:,} bytes")
    print(f"    Child Count: {node_12400['child_count']}")
    print(f"    Offset: 0x{node_12400['offset']:X}")

    # Extract binaries
    header_size = 12

    # Frontiers 0x22400
    offset_22400 = node_22400['offset']
    total_size_22400 = header_size + node_22400['data_size']
    if offset_22400 + total_size_22400 > len(esf_frontiers):
        print(f"[-] 0x22400 node extends beyond file")
        sys.exit(1)
    binary_22400 = esf_frontiers[offset_22400:offset_22400 + total_size_22400]

    # Vanilla 0x12400
    offset_12400 = node_12400['offset']
    total_size_12400 = header_size + node_12400['data_size']
    if offset_12400 + total_size_12400 > len(esf_vanilla):
        print(f"[-] 0x12400 node extends beyond file")
        sys.exit(1)
    binary_12400 = esf_vanilla[offset_12400:offset_12400 + total_size_12400]

    # Comparison
    print("\n" + "=" * 100)
    print("COMPARISON REPORT")
    print("=" * 100)

    print(f"\n[*] TYPE ID COMPARISON:")
    print(f"    Frontiers (0x22400): 0x{node_22400['type_id']:05X}")
    print(f"    Vanilla (0x12400):   0x{node_12400['type_id']:05X}")
    print(f"    [!] TYPE IDs DIFFER - Type ID conversion REQUIRED")

    print(f"\n[*] DATA SIZE COMPARISON:")
    print(f"    Frontiers: {node_22400['data_size']:,} bytes")
    print(f"    Vanilla:   {node_12400['data_size']:,} bytes")
    print(f"    Match:     {node_22400['data_size'] == node_12400['data_size']}")
    if node_22400['data_size'] != node_12400['data_size']:
        diff = node_22400['data_size'] - node_12400['data_size']
        print(f"    Difference: {diff:+,} bytes")

    print(f"\n[*] TOTAL BINARY SIZE COMPARISON:")
    print(f"    Frontiers: {len(binary_22400):,} bytes")
    print(f"    Vanilla:   {len(binary_12400):,} bytes")

    # Binary comparison
    print(f"\n[*] BINARY DATA COMPARISON:")

    min_len = min(len(binary_22400), len(binary_12400))
    differences = 0
    first_diff_offset = None
    diff_map = []

    for i in range(min_len):
        if binary_22400[i] != binary_12400[i]:
            if first_diff_offset is None:
                first_diff_offset = i
            differences += 1
            if len(diff_map) < 20:
                diff_map.append((i, binary_22400[i], binary_12400[i]))

    pct_diff = (100 * differences / min_len) if min_len > 0 else 0

    print(f"    Total bytes compared: {min_len:,}")
    print(f"    Bytes that differ: {differences:,} ({pct_diff:.2f}%)")

    if differences > 0:
        print(f"\n    First difference at byte offset 0x{first_diff_offset:X}:")
        print(f"      Frontiers (0x22400): 0x{binary_22400[first_diff_offset]:02X}")
        print(f"      Vanilla (0x12400):   0x{binary_12400[first_diff_offset]:02X}")
        print(f"\n    First 20 byte differences:")
        for offset, f_byte, v_byte in diff_map:
            print(f"      0x{offset:X}: 0x22400=0x{f_byte:02X}, 0x12400=0x{v_byte:02X}")

    if len(binary_22400) != len(binary_12400):
        print(f"\n    Binary size mismatch:")
        print(f"      Frontiers: {len(binary_22400):,} bytes")
        print(f"      Vanilla:   {len(binary_12400):,} bytes")

    # Hex dumps
    print_hex_dump(
        binary_22400,
        "\n[*] FRONTIERS 0x22400 NODE - First 512 bytes",
        start_offset=offset_22400
    )

    print_hex_dump(
        binary_12400,
        "\n[*] VANILLA 0x12400 NODE - First 512 bytes",
        start_offset=offset_12400
    )

    # Analysis
    print("\n" + "=" * 100)
    print("ANALYSIS & RECOMMENDATIONS")
    print("=" * 100)

    print("\n[*] STRUCTURAL DIFFERENCES IDENTIFIED:")
    print(f"    1. Type ID differs: 0x12400 (Vanilla) -> 0x22400 (Frontiers)")
    print(f"    2. Data size differs: {node_12400['data_size']:,} -> {node_22400['data_size']:,} bytes")

    if differences == 0 and len(binary_22400) == len(binary_12400):
        print(f"    3. Binary data is IDENTICAL (after header)")
        print(f"\n[!] CONCLUSION: Minimal conversion required.")
        print(f"    - Type ID change: 0x12400 -> 0x22400")
        print(f"    - Data layout: NO CHANGE required")
        print(f"    - Binary structure: Compatible as-is")
    elif pct_diff < 20:
        print(f"    3. Binary data is mostly similar ({100-pct_diff:.1f}% match)")
        print(f"\n[!] CONCLUSION: Targeted field mapping required.")
        print(f"    - Type ID change: 0x12400 -> 0x22400")
        print(f"    - Find the {differences} differing bytes and understand their purpose")
        print(f"    - Implement selective field updates")
    else:
        print(f"    3. Binary data is significantly different ({pct_diff:.1f}% differ)")
        print(f"\n[!] CONCLUSION: Full data structure conversion needed.")
        print(f"    - Type ID change: 0x12400 -> 0x22400")
        print(f"    - Binary data requires interpretation and remapping")
        print(f"    - Implement field-by-field conversion logic")

    print("\n" + "=" * 100)


if __name__ == '__main__':
    main()
