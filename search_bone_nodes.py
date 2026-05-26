#!/usr/bin/env python3
"""
Bone Node Type Search
=====================
Search ESF files for type IDs 0x22400 and 0x12400 to locate bone nodes.
"""

import os
import sys
import struct

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'core'))
from esf_parser import ESFParser


def search_for_type_ids(node, type_ids, results=None, path="root", depth=0):
    """Recursively search tree for specific type IDs."""
    if results is None:
        results = []

    if node['type_id'] in type_ids:
        results.append({
            'type_id': node['type_id'],
            'path': path,
            'depth': depth,
            'data_size': node['data_size'],
            'child_count': node['child_count'],
            'offset': node['offset'],
            'node': node
        })

    for i, child in enumerate(node['children']):
        search_for_type_ids(child, type_ids, results, f"{path}/child_{i}", depth + 1)

    return results


def main():
    esf_files = [
        ('workspace/expansion/CHAR.ESF', 'Frontiers'),
        ('workspace/original/CHAR.ESF', 'Vanilla')
    ]

    target_types = [0x22400, 0x12400]

    print("=" * 100)
    print("BONE NODE TYPE SEARCH")
    print("=" * 100)

    for esf_path, label in esf_files:
        print(f"\n[*] Searching {label}: {esf_path}")

        if not os.path.exists(esf_path):
            print(f"[-] File not found: {esf_path}")
            continue

        with open(esf_path, 'rb') as f:
            esf_data = f.read()

        print(f"    File size: {len(esf_data):,} bytes")

        try:
            parser = ESFParser(esf_data)
            parser.parse()
            print(f"[+] Parsed successfully")

        except Exception as e:
            print(f"[-] Parse failed: {e}")
            continue

        # Search entire tree
        print(f"\n[*] Searching for types: {', '.join(f'0x{t:05X}' for t in target_types)}")
        results = search_for_type_ids(parser.root, target_types)

        if not results:
            print(f"[-] No bone node types found!")
            print(f"\n[*] Let's search for any other interesting container types...")

            # Count all unique type IDs
            type_counts = {}

            def count_types(node):
                t = node['type_id']
                type_counts[t] = type_counts.get(t, 0) + 1
                for child in node['children']:
                    count_types(child)

            count_types(parser.root)

            # Sort by count and show top 20
            sorted_types = sorted(type_counts.items(), key=lambda x: x[1], reverse=True)
            print("\n    Top 20 most common type IDs:")
            for type_id, count in sorted_types[:20]:
                print(f"      0x{type_id:05X}: {count} nodes")

        else:
            print(f"[+] Found {len(results)} bone node(s)!")
            for idx, result in enumerate(results):
                print(f"\n    [{idx}] Type: 0x{result['type_id']:05X}")
                print(f"        Path: {result['path']}")
                print(f"        Depth: {result['depth']}")
                print(f"        Data Size: {result['data_size']:,} bytes")
                print(f"        Child Count: {result['child_count']}")
                print(f"        Offset: 0x{result['offset']:X}")


if __name__ == '__main__':
    main()
