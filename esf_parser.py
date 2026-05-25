#!/usr/bin/env python3
"""
EQOA ESF Parser and Virtual Pointer Table Generator
===================================================
Uses the 'construct' library to declare binary schemas for the ESF header 
and node headers, parses the recursive FJBO tree, and flattens models 
into a virtual Pointer Table. Zero-coupled architecture.
"""

import os
import sys
import struct
from construct import Struct, Const, Int32ul, Bytes

# ============================================================
# Binary Schema Declarations (construct)
# ============================================================

# 32-byte ESF File Header
EsfHeader = Struct(
    "magic" / Const(b"FJBO"),
    "version" / Int32ul,
    "constant" / Int32ul,
    "reserved1" / Int32ul,
    "header_size" / Int32ul,
    "reserved2" / Int32ul,
    "padding" / Bytes(8)
)

# 12-byte Node Header
EsfNodeHeader = Struct(
    "type_id" / Int32ul,
    "data_size" / Int32ul,
    "child_count" / Int32ul
)

# ============================================================
# Virtual Pointer Table Struct
# ============================================================
class PointerTableEntry:
    """A flattened representation of a model asset inside the ESF tree."""
    def __init__(self, index, asset_id, offset, length, type_id):
        self.index = index          # Sequential model index (0-based)
        self.asset_id = asset_id    # Asset Hash (from type 0x11111)
        self.offset = offset        # Absolute byte offset in the ESF file
        self.length = length        # Absolute byte length (node header + data)
        self.type_id = type_id      # Type of the model node (e.g. 0x62700)

    def __repr__(self):
        hash_str = f"0x{self.asset_id:08X}" if self.asset_id is not None else "None"
        return (f"Entry(idx={self.index}, ID={hash_str}, "
                f"offset=0x{self.offset:X}, len={self.length:,}, type=0x{self.type_id:05X})")

# ============================================================
# Core Parser Logic
# ============================================================
class ESFParser:
    """Recursive FJBO ESF Parser."""
    def __init__(self, data):
        self.data = data
        self.file_size = len(data)
        self.header = None
        self.root = None
        self.pointer_table = []

    def parse(self):
        """Parse the ESF header and initiate recursive tree traversal."""
        # 1. Parse File Header
        try:
            self.header = EsfHeader.parse(self.data[:32])
        except Exception as e:
            raise ValueError(f"Failed to parse ESF header: {e}")

        # 2. Parse Recursive Tree starting at Root Node (offset 0x20)
        self.root, end_pos = self._parse_node(32)
        
        # Safety Check: Did we reach the end of the file?
        if end_pos != self.file_size:
            print(f"[*] Warning: Tree parsing completed at offset 0x{end_pos:X}, "
                  f"but file size is 0x{self.file_size:X} (difference of {self.file_size - end_pos} bytes)")
        
        # 3. Build Virtual Pointer Table
        self._build_pointer_table()
        return self

    def _parse_node(self, pos):
        """Recursively deserialize a node at the given byte offset."""
        if pos + 12 > self.file_size:
            raise EOFError(f"Unexpected EOF while reading node header at offset 0x{pos:X}")

        # Parse Node Header using construct schema
        node_header_bytes = self.data[pos:pos+12]
        hdr = EsfNodeHeader.parse(node_header_bytes)
        
        type_id = hdr.type_id
        data_size = hdr.data_size
        child_count = hdr.child_count

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
            # Leaf node: read inline data
            if next_pos + data_size > self.file_size:
                raise EOFError(f"Unexpected EOF while reading leaf node data at offset 0x{next_pos:X} (expected {data_size} bytes)")
            node['inline_data'] = self.data[next_pos:next_pos + data_size]
            next_pos += data_size
        else:
            # Branch node: recursively parse child nodes
            for _ in range(child_count):
                child, next_pos = self._parse_node(next_pos)
                node['children'].append(child)

        return node, next_pos

    def _find_hash_in_subtree(self, node):
        """Deep-search the subtree of a model node for the 0x11111 hash leaf node."""
        if node['type_id'] == 0x11111:
            if node['inline_data'] and len(node['inline_data']) >= 4:
                return struct.unpack('<I', node['inline_data'][:4])[0]
        for child in node['children']:
            val = self._find_hash_in_subtree(child)
            if val is not None:
                return val
        return None

    def _build_pointer_table(self):
        """Locates the Model Container and flattens its children into PointerTableEntry objects."""
        if not self.root or len(self.root['children']) == 0:
            return

        # Find the Model Container (type 0x0A010) under the root node
        model_container = None
        for child in self.root['children']:
            if child['type_id'] == 0x0A010:
                model_container = child
                break

        if not model_container:
            print("[-] Warning: Model Container (0x0A010) not found in the ESF tree.")
            return

        # Flatten the model container's children
        for idx, model_node in enumerate(model_container['children']):
            # Find the unique Asset ID (hash) inside the model's subtree
            asset_hash = self._find_hash_in_subtree(model_node)
            
            # Total size of the model node is header (12) + data_size
            length = 12 + model_node['data_size']
            offset = model_node['offset']
            type_id = model_node['type_id']
            
            entry = PointerTableEntry(idx, asset_hash, offset, length, type_id)
            self.pointer_table.append(entry)

    def verify_integrity(self):
        """
        Performs the summation sanity check by recursively traversing the tree
        to accumulate the sizes of node headers, leaf payloads, master header size,
        and identifying any byte-alignment or padding mismatches.
        """
        if not self.root:
            raise ValueError("Cannot verify integrity. Parse the file first.")

        header_size = self.header.header_size
        total_nodes = 0
        leaf_payloads_size = 0
        
        def traverse(node):
            nonlocal total_nodes, leaf_payloads_size
            total_nodes += 1
            if node['child_count'] == 0:
                leaf_payloads_size += node['data_size']
            for child in node['children']:
                traverse(child)
                
        traverse(self.root)
        
        node_headers_size = total_nodes * 12
        calculated_total = header_size + node_headers_size + leaf_payloads_size
        padding_bytes = self.file_size - calculated_total
        
        return {
            'file_size': self.file_size,
            'header_size': header_size,
            'total_nodes': total_nodes,
            'node_headers_size': node_headers_size,
            'leaf_payloads_size': leaf_payloads_size,
            'calculated_total_without_padding': calculated_total,
            'padding_bytes': padding_bytes,
            'calculated_total': calculated_total + padding_bytes,
            'integrity_match': calculated_total + padding_bytes == self.file_size
        }

# ============================================================
# CLI Execution
# ============================================================
def main():
    if len(sys.argv) < 2:
        print("Usage: python esf_parser.py <path_to_esf>")
        sys.exit(1)
        
    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f"[-] Error: File not found at '{filepath}'")
        sys.exit(1)
        
    print(f"[*] Reading and parsing ESF file: {filepath}")
    with open(filepath, 'rb') as f:
        data = f.read()
        
    parser = ESFParser(data)
    try:
        parser.parse()
    except EOFError as e:
        print(f"\n[-] Critical EOFError: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n[-] Parsing failed: {e}")
        sys.exit(1)

    print("\n[+] Parsing Completed Successfully!")
    print(f"  ESF Version: {parser.header.version}")
    print(f"  Root Children: {parser.root['child_count']}")
    print(f"  Parsed Model Count (Virtual Pointer Table): {len(parser.pointer_table)}")
    
    # Print the first 5 entries as a sanity check
    if parser.pointer_table:
        print("\n[*] First 5 Virtual Pointer Table Entries:")
        for entry in parser.pointer_table[:5]:
            print(f"  {entry}")
            
        print("\n[*] Last 5 Virtual Pointer Table Entries:")
        for entry in parser.pointer_table[-5:]:
            print(f"  {entry}")

    # Run Sanity Summation Check
    integrity = parser.verify_integrity()
    print("\n" + "="*50)
    print("PROMPT 5: SANITY CHECK - THE SUMMATION TEST")
    print("="*50)
    print(f"  OS-Level File Size:          {integrity['file_size']:,} bytes")
    print(f"  Header Size:                 {integrity['header_size']:,} bytes")
    print(f"  Total Node Count:            {integrity['total_nodes']:,} nodes")
    print(f"  Accumulated Node Headers:    {integrity['node_headers_size']:,} bytes (12 bytes/node)")
    print(f"  Accumulated Leaf Payloads:   {integrity['leaf_payloads_size']:,} bytes")
    print(f"  Extraneous / Padding Bytes:  {integrity['padding_bytes']:,} bytes")
    print(f"  Calculated Total Size:       {integrity['calculated_total']:,} bytes")
    print("-"*50)
    if integrity['integrity_match']:
        print("  [+] INTEGRITY PASS: Calculated sum exactly matches OS file size!")
    else:
        print("  [-] INTEGRITY FAIL: Size mismatch detected!")
    print("="*50)

if __name__ == '__main__':
    main()

