#!/usr/bin/env python3
"""
Bone Node Comparison Tool
=========================
Extracts CHAR.ESF from both EQOA_Frontiers.iso and EQOA_Original.iso,
parses the first character model, locates Child 6 (bone node container),
and compares Type ID, size, and binary data between the two versions.
"""

import os
import sys
import struct
from io import BytesIO

try:
    import pycdlib
except ImportError:
    print("[-] Error: pycdlib not installed. Install with: pip install pycdlib")
    sys.exit(1)

# Add core module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'core'))
from esf_parser import ESFParser


def extract_char_esf_from_iso(iso_path):
    """Extract CHAR.ESF from an ISO file using pycdlib."""
    print(f"\n[*] Opening ISO: {iso_path}")

    if not os.path.exists(iso_path):
        print(f"[-] Error: ISO file not found: {iso_path}")
        return None

    try:
        iso = pycdlib.PyCdlib()
        iso.open(iso_path)

        # Helper to find CHAR.ESF recursively
        def find_char_esf(iso, path='/'):
            try:
                for child in iso.list_dir(path):
                    child_name = child.file_identifier()
                    if isinstance(child_name, bytes):
                        child_name_str = child_name.decode('utf-8', errors='ignore')
                    else:
                        child_name_str = str(child_name)

                    if child_name_str.upper() == 'CHAR.ESF;1':
                        full_path = path.rstrip('/') + '/' + child_name_str
                        return full_path

                    if not child.is_cl_to_moved_dr() and child.is_dir() and child_name_str not in ['.', '..']:
                        full_path = path.rstrip('/') + '/' + child_name_str
                        result = find_char_esf(iso, full_path)
                        if result:
                            return result
            except Exception as e:
                pass

            return None

        char_esf_path = find_char_esf(iso)

        if char_esf_path:
            print(f"[*] Found CHAR.ESF at: {char_esf_path}")
            char_esf_data = BytesIO()
            iso.get_file_from_iso(char_esf_data, iso_path=char_esf_path)
            size = char_esf_data.tell()
            print(f"[+] Successfully extracted CHAR.ESF (size: {size:,} bytes)")
            iso.close()
            return char_esf_data.getvalue()
        else:
            print(f"[-] Could not find CHAR.ESF in ISO")

            # List root directory for debugging
            print("[*] Listing ISO root directory:")
            try:
                for child in iso.list_dir('/'):
                    child_name = child.file_identifier()
                    if isinstance(child_name, bytes):
                        child_name_str = child_name.decode('utf-8', errors='ignore')
                    else:
                        child_name_str = str(child_name)
                    print(f"  {child_name_str}")
            except Exception as e:
                print(f"  Error listing: {e}")

        iso.close()
        return None

    except Exception as e:
        print(f"[-] Error opening ISO: {e}")
        import traceback
        traceback.print_exc()
        return None


def print_hex_dump(data, label, max_bytes=512):
    """Print hex dump of binary data."""
    print(f"\n{label}")
    print("=" * 100)

    bytes_to_show = min(len(data), max_bytes)

    for offset in range(0, bytes_to_show, 16):
        chunk = data[offset:offset + 16]
        hex_str = ' '.join(f'{b:02x}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        print(f"  0x{offset:06x}: {hex_str:<48} | {ascii_str}")

    if len(data) > max_bytes:
        print(f"  ... ({len(data) - max_bytes} more bytes)")


def traverse_esf_tree(node, depth=0):
    """Recursively traverse ESF tree and return formatted structure."""
    indent = "  " * depth
    type_id = node['type_id']
    data_size = node['data_size']
    child_count = node['child_count']

    node_info = f"{indent}Type: 0x{type_id:05X}, Size: {data_size:,} bytes, Children: {child_count}"

    results = [node_info]
    for i, child in enumerate(node['children']):
        results.extend(traverse_esf_tree(child, depth + 1))

    return results


def find_bone_node_by_type(esf_data, iso_name, target_type):
    """
    Parse ESF and locate the first node with type_id == target_type anywhere in the ESF tree.
    Returns a dict with serialized bytes for the located node subgraph.
    """
    print(f"\n[*] Parsing ESF from {iso_name}...")
    try:
        parser = ESFParser(esf_data)
        parser.parse()
        print(f"[+] ESF parsed successfully!")
        print(f"    Total nodes: {parser.verify_integrity()['total_nodes']}")
        print(f"    Root children: {parser.root['child_count']}")
    except Exception as e:
        print(f"[-] Failed to parse ESF: {e}")
        return None

    if not parser.root:
        print("[-] ESF has no root node")
        return None

    def dfs_find(node, depth=0, path="root"):
        if node['type_id'] == target_type:
            return {
                'node': node,
                'path': path,
                'depth': depth
            }
        for i, ch in enumerate(node['children']):
            res = dfs_find(ch, depth + 1, f"{path}/child_{i}")
            if res:
                return res
        return None

    print(f"[*] Searching for bone node type: 0x{target_type:05X} (DFS across entire tree)...")
    found = dfs_find(parser.root)

    if not found:
        print(f"[-] Could not find any node with type_id == 0x{target_type:05X} in {iso_name}")
        return None

    bone_node = found['node']
    bone_path = found.get('path')
    bone_depth = found.get('depth')

    print(f"\n[+] Located bone node:")
    print(f"    Type ID: 0x{bone_node['type_id']:05X}")
    print(f"    Size (data_size): {bone_node['data_size']:,} bytes")
    print(f"    Children: {bone_node['child_count']}")
    print(f"    Offset in ESF: 0x{bone_node['offset']:X}")
    if bone_path:
        print(f"    Path in ESF tree: {bone_path}")
    if bone_depth is not None:
        print(f"    Depth: {bone_depth}")

    header_size = 12
    node_offset = bone_node['offset']

    def leaf_serialized_len(n):
        return header_size + n['data_size']

    def serialized_end_offset(node):
        start = node['offset']
        if node['child_count'] == 0:
            return start + leaf_serialized_len(node)

        ends = [serialized_end_offset(ch) for ch in node['children']]
        if not ends:
            return start + header_size
        return max(ends)

    node_end = serialized_end_offset(bone_node)
    total_size = node_end - node_offset

    if node_end > len(esf_data):
        print(f"[-] Computed node end (0x{node_end:X}) exceeds file bounds (0x{len(esf_data):X})")
        return None

    node_binary = esf_data[node_offset:node_end]

    return {
        'type_id': bone_node['type_id'],
        'data_size': bone_node['data_size'],
        'child_count': bone_node['child_count'],
        'offset': node_offset,
        'total_size': total_size,
        'binary_data': node_binary,
        'iso_name': iso_name,
        'bone_node_tree': bone_node,
    }


def main():
    esf_frontiers_path = 'workspace/expansion/CHAR.ESF'
    esf_original_path = 'workspace/original/CHAR.ESF'

    print("=" * 100)
    print("EQOA BONE NODE COMPARISON TOOL")
    print("=" * 100)

    # Load ESF files
    print(f"\n[*] Step 1: Loading pre-extracted CHAR.ESF files")

    if not os.path.exists(esf_frontiers_path):
        print(f"[-] Frontiers ESF not found: {esf_frontiers_path}")
        sys.exit(1)

    if not os.path.exists(esf_original_path):
        print(f"[-] Original ESF not found: {esf_original_path}")
        sys.exit(1)

    print(f"[+] Loading {esf_frontiers_path}")
    with open(esf_frontiers_path, 'rb') as f:
        esf_frontiers = f.read()
    print(f"    Size: {len(esf_frontiers):,} bytes")

    print(f"[+] Loading {esf_original_path}")
    with open(esf_original_path, 'rb') as f:
        esf_original = f.read()
    print(f"    Size: {len(esf_original):,} bytes")

    # Parse and extract bone nodes by type anywhere in the ESF tree
    print(f"\n[*] Step 2: Parsing ESF files and extracting bone nodes by type_id")

    bone_frontiers = find_bone_node_by_type(esf_frontiers, "EQOA_Frontiers.iso", 0x22400)
    if not bone_frontiers:
        print("[-] Failed to extract bone node from Frontiers (type_id 0x22400)")
        sys.exit(1)

    bone_original = find_bone_node_by_type(esf_original, "EQOA_Original.iso", 0x12400)
    if not bone_original:
        print("[-] Failed to extract bone node from Original (type_id 0x12400)")
        sys.exit(1)

    # Compare bone nodes
    print("\n" + "=" * 100)
    print("BONE NODE COMPARISON RESULTS")
    print("=" * 100)

    print(f"\n[*] FRONTIERS (Expected Type 0x22400)")
    print(f"    Type ID:      0x{bone_frontiers['type_id']:05X}")
    print(f"    Data Size:    {bone_frontiers['data_size']:,} bytes")
    print(f"    Child Count:  {bone_frontiers['child_count']}")
    print(f"    Total Size:   {bone_frontiers['total_size']:,} bytes")

    print(f"\n[*] VANILLA (Expected Type 0x12400)")
    print(f"    Type ID:      0x{bone_original['type_id']:05X}")
    print(f"    Data Size:    {bone_original['data_size']:,} bytes")
    print(f"    Child Count:  {bone_original['child_count']}")
    print(f"    Total Size:   {bone_original['total_size']:,} bytes")

    # Structural comparison
    print(f"\n[*] STRUCTURAL DIFFERENCES:")

    type_match = bone_frontiers['type_id'] == bone_original['type_id']
    print(f"    Type IDs match:     {type_match} "
          f"(Frontiers: 0x{bone_frontiers['type_id']:05X} vs Vanilla: 0x{bone_original['type_id']:05X})")

    size_match = bone_frontiers['data_size'] == bone_original['data_size']
    print(f"    Data sizes match:   {size_match} "
          f"(Frontiers: {bone_frontiers['data_size']:,} vs Vanilla: {bone_original['data_size']:,})")

    children_match = bone_frontiers['child_count'] == bone_original['child_count']
    print(f"    Child counts match: {children_match} "
          f"(Frontiers: {bone_frontiers['child_count']} vs Vanilla: {bone_original['child_count']})")

    # Binary comparison
    print(f"\n[*] BINARY DATA COMPARISON:")

    min_len = min(len(bone_frontiers['binary_data']), len(bone_original['binary_data']))
    differences = 0
    first_diff_offset = None

    for i in range(min_len):
        if bone_frontiers['binary_data'][i] != bone_original['binary_data'][i]:
            if first_diff_offset is None:
                first_diff_offset = i
            differences += 1

    print(f"    Total bytes compared: {min_len:,}")
    print(f"    Bytes that differ: {differences} ({100*differences/min_len:.2f}%)")

    if first_diff_offset is not None:
        print(f"    First difference at offset: 0x{first_diff_offset:X}")
        print(f"      Frontiers byte: 0x{bone_frontiers['binary_data'][first_diff_offset]:02X}")
        print(f"      Vanilla byte:   0x{bone_original['binary_data'][first_diff_offset]:02X}")

    if len(bone_frontiers['binary_data']) != len(bone_original['binary_data']):
        print(f"\n    Size mismatch:")
        print(f"      Frontiers total: {len(bone_frontiers['binary_data']):,} bytes")
        print(f"      Vanilla total:   {len(bone_original['binary_data']):,} bytes")
        print(f"      Difference:      {abs(len(bone_frontiers['binary_data']) - len(bone_original['binary_data'])):,} bytes")

    def collect_leaf_payloads(node, path="root"):
        """
        Collect leaf nodes under the given node.
        Returns list of dicts in DFS order:
          { type_id, data_size, path, offset, inline_data }
        """
        out = []
        if node['child_count'] == 0:
            out.append({
                'type_id': node['type_id'],
                'data_size': node['data_size'],
                'path': path,
                'offset': node['offset'],
                'inline_data': node['inline_data'],
            })
            return out

        for i, ch in enumerate(node['children']):
            out.extend(collect_leaf_payloads(ch, f"{path}/child_{i}"))
        return out

    def diff_bytes(a: bytes, b: bytes):
        min_len = min(len(a), len(b))
        diffs = 0
        first = None
        for i in range(min_len):
            if a[i] != b[i]:
                if first is None:
                    first = i
                diffs += 1
        pct = (100 * diffs / min_len) if min_len else 0.0
        return {
            'min_len': min_len,
            'diffs': diffs,
            'pct': pct,
            'first_diff_offset': first,
            'len_a': len(a),
            'len_b': len(b),
        }

    print(f"\n[*] LEAF PAYLOAD DIFF SUMMARY (DFS order):")

    leaves_front = collect_leaf_payloads(bone_frontiers['bone_node_tree'])
    leaves_van = collect_leaf_payloads(bone_original['bone_node_tree'])

    print(f"    Frontiers leaf count: {len(leaves_front)}")
    print(f"    Vanilla   leaf count: {len(leaves_van)}")

    # Compare leaves by index in DFS order (best-effort structural alignment)
    leaf_cmp_len = min(len(leaves_front), len(leaves_van))
    leaf_diffs = 0
    first_leaf_diff = None
    leaf_report_rows = []

    for i in range(leaf_cmp_len):
        lf = leaves_front[i]
        lv = leaves_van[i]

        same_type = lf['type_id'] == lv['type_id']
        d = diff_bytes(lf['inline_data'], lv['inline_data'])

        if d['diffs'] != 0 or not same_type or lf['data_size'] != lv['data_size']:
            leaf_diffs += 1
            if first_leaf_diff is None:
                first_leaf_diff = i

            leaf_report_rows.append({
                'idx': i,
                'front_type': lf['type_id'],
                'van_type': lv['type_id'],
                'front_size': lf['data_size'],
                'van_size': lv['data_size'],
                'front_path': lf['path'],
                'van_path': lv['path'],
                'diffs': d['diffs'],
                'pct': d['pct'],
                'first_diff_offset': d['first_diff_offset'],
                'len_front': d['len_a'],
                'len_van': d['len_b'],
            })

    print(f"    Compared leaf payloads: {leaf_cmp_len}")
    print(f"    Leaf payloads differing (best-effort): {leaf_diffs}")
    if first_leaf_diff is not None:
        print(f"    First differing leaf payload index: {first_leaf_diff}")
    else:
        print(f"    [+] Leaf payloads identical (best-effort by DFS order)")

    # Write deterministic report artifact
    report_path = "BONE_NODE_COMPARISON_REPORT.md"
    with open(report_path, "w", encoding="utf-8") as rf:
        rf.write("# Bone Node Comparison Report (Frontiers vs Vanilla)\n\n")
        rf.write("## Summary\n")
        rf.write(f"- Frontiers extracted type_id: `0x{bone_frontiers['type_id']:05X}`\n")
        rf.write(f"- Vanilla extracted type_id:   `0x{bone_original['type_id']:05X}`\n")
        rf.write(f"- Frontiers data_size: {bone_frontiers['data_size']:,} bytes\n")
        rf.write(f"- Vanilla data_size:   {bone_original['data_size']:,} bytes\n")
        rf.write(f"- Frontiers child_count: {bone_frontiers['child_count']}\n")
        rf.write(f"- Vanilla child_count:   {bone_original['child_count']}\n")
        rf.write(f"- Frontiers serialized total_size: {bone_frontiers['total_size']:,} bytes\n")
        rf.write(f"- Vanilla serialized total_size:   {bone_original['total_size']:,} bytes\n\n")

        rf.write("## Binary Comparison (serialized bone-node subgraph)\n")
        rf.write(f"- Frontiers serialized length: {len(bone_frontiers['binary_data']):,} bytes\n")
        rf.write(f"- Vanilla serialized length:   {len(bone_original['binary_data']):,} bytes\n")
        rf.write(f"- Bytes compared: {min_len:,}\n")
        rf.write(f"- Differing bytes: {differences:,}\n")
        rf.write(f"- Percent difference: {(100 * differences / min_len) if min_len else 0.0:.2f}%\n\n")

        if first_diff_offset is not None:
            rf.write("### First byte difference\n")
            rf.write(f"- Byte offset (within compared serialized span): `0x{first_diff_offset:X}`\n")
            rf.write(f"- Frontiers byte: `0x{bone_frontiers['binary_data'][first_diff_offset]:02X}`\n")
            rf.write(f"- Vanilla byte:   `0x{bone_original['binary_data'][first_diff_offset]:02X}`\n\n")

        rf.write("## Leaf Payload Comparison (DFS order, best-effort alignment)\n")
        rf.write(f"- Frontiers leaf count: {len(leaves_front)}\n")
        rf.write(f"- Vanilla   leaf count: {len(leaves_van)}\n")
        rf.write(f"- Compared leaf payloads: {leaf_cmp_len}\n")
        rf.write(f"- Differing leaf payloads (best-effort): {leaf_diffs}\n\n")

        rf.write("|#|Front Type|Van Type|Front Size|Van Size|Diffs|Diff%|FirstDiff@|Front Path|Van Path|\n")
        rf.write("|-:|---:|---:|---:|---:|---:|---:|---:|---|---|\n")
        for r in leaf_report_rows[:200]:
            rf.write(
                f"|{r['idx']}|0x{r['front_type']:05X}|0x{r['van_type']:05X}|{r['front_size']:,}|{r['van_size']:,}|"
                f"{r['diffs']:,}|{r['pct']:.1f}%|{('0x%X'%r['first_diff_offset']) if r['first_diff_offset'] is not None else '—'}|"
                f"{r['front_path']}|{r['van_path']}|\n"
            )

    print(f"\n[+] Wrote report: {report_path}")

    # Print hex dumps
    print_hex_dump(bone_frontiers['binary_data'],
                   "\n[*] FRONTIERS BONE NODE - First 512 bytes (hex dump)")

    print_hex_dump(bone_original['binary_data'],
                   "\n[*] VANILLA BONE NODE - First 512 bytes (hex dump)")

    # Summary
    print("\n" + "=" * 100)
    print("ANALYSIS SUMMARY")
    print("=" * 100)

    if bone_frontiers['type_id'] != bone_original['type_id']:
        print("\n[!] TYPE ID DIFFERS - Data structure conversion is REQUIRED")
        print(f"    Must convert type 0x{bone_original['type_id']:05X} -> 0x{bone_frontiers['type_id']:05X}")
    else:
        print("\n[!] TYPE IDs are IDENTICAL - Check if type ID change is required anyway")

    if differences > 0 and differences == min_len:
        print("[!] WARNING: Binary data is COMPLETELY DIFFERENT")
        print("    This indicates a complete data structure overhaul is needed")
    elif differences == 0:
        print("[!] Binary data is IDENTICAL - No conversion needed, only type ID change")
    else:
        print(f"[!] Binary data is PARTIALLY DIFFERENT ({100*differences/min_len:.1f}%)")
        print("    Conversion logic needed to map old structure to new structure")

    print("\n" + "=" * 100)


if __name__ == '__main__':
    main()
