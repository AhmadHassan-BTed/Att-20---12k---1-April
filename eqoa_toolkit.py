#!/usr/bin/env python3
"""
EQOA ESF/CSF File Format Toolkit
=================================
Tools for analyzing, extracting, and patching EverQuest Online Adventures
ESF (FJBO) and CSF (CESF) game asset files.

ESF = Uncompressed asset archive (magic: "FJBO")
  - Internal format: recursive typed tree of nodes
  - Each node: (type_id:u32, data_size:u32, child_count:u32)
  - Leaf nodes (count=0): followed by data_size bytes of inline data
  - Branch nodes (count>0): followed by child_count child nodes
  - Tree covers the ENTIRE file after the 32-byte header
  - type_id identifies the data format (NOT a file offset)

CSF = Block-compressed ESF (magic: "CESF", zlib 256KB blocks)

Usage:
    python eqoa_toolkit.py info <file.esf|file.csf>
    python eqoa_toolkit.py tree <file.esf> [max_depth]
    python eqoa_toolkit.py models <file.esf>
    python eqoa_toolkit.py extract_model <file.esf> <index> <output>
    python eqoa_toolkit.py decompress <file.csf> [output.esf]
    python eqoa_toolkit.py compress <file.esf> [output.csf]
    python eqoa_toolkit.py compare <file1.esf> <file2.esf>
    python eqoa_toolkit.py extract_all <iso_mount_dir> <output_dir>
    python eqoa_toolkit.py dump <file.esf> [offset] [length]
"""

import struct
import zlib
import os
import sys
import hashlib
from pathlib import Path


# ============================================================
# CSF (CESF) Format - Block-compressed ESF
# ============================================================
# Header (40 bytes):
#   0x00: "CESF" magic (4 bytes)
#   0x04: uint32 block_count
#   0x08: uint32 compressed_data_size
#   0x0C: uint32 reserved (0)
#   0x10: uint32 decompressed_size (= ESF file size)
#   0x14: uint32 reserved (0)
#   0x18: uint32 header_size (40)
#   0x1C: uint32 reserved (0)
#   0x20: uint32 first_block_compressed_size
#   0x24: uint32 checksum/hash
#   0x28: uint32 first_block_compressed_size (duplicate?)
#   0x2C: uint32 block_size (262144 = 256KB)
# Data:
#   Block 0: zlib compressed data (78 DA header)
#   [8-byte inter-block header: compressed_size, decompressed_size]
#   Block 1: zlib compressed data
#   ... repeat ...

CSF_MAGIC = b'CESF'
ESF_MAGIC = b'FJBO'
CSF_HEADER_SIZE = 40
CSF_BLOCK_SIZE = 262144  # 256KB default


class CSFFile:
    """Parser for CESF (compressed ESF) files."""

    def __init__(self, filepath):
        self.filepath = filepath
        with open(filepath, 'rb') as f:
            self.raw_data = f.read()
        self._parse_header()

    def _parse_header(self):
        if self.raw_data[:4] != CSF_MAGIC:
            raise ValueError("Not a CSF file (expected CESF magic)")

        self.block_count = struct.unpack_from('<I', self.raw_data, 0x04)[0]
        self.compressed_size = struct.unpack_from('<I', self.raw_data, 0x08)[0]
        self.decompressed_size = struct.unpack_from('<I', self.raw_data, 0x10)[0]
        self.header_size = struct.unpack_from('<I', self.raw_data, 0x18)[0]
        self.first_block_csize = struct.unpack_from('<I', self.raw_data, 0x20)[0]
        self.checksum = struct.unpack_from('<I', self.raw_data, 0x24)[0]
        self.block_size = struct.unpack_from('<I', self.raw_data, 0x2C)[0]

    def decompress(self):
        """Decompress CSF to ESF data."""
        pos = 0x30  # After 48-byte header (40 + 8 for first block header?)
        # Actually the data starts right at 0x30
        result = bytearray()

        for i in range(self.block_count):
            decomp = zlib.decompressobj()
            remaining = self.raw_data[pos:]
            block_data = decomp.decompress(remaining)
            consumed = len(remaining) - len(decomp.unused_data)
            result.extend(block_data)
            pos += consumed

            # Read 8-byte inter-block header (except after last block)
            if i < self.block_count - 1:
                pos += 8  # Skip inter-block header

        return bytes(result)

    def info(self):
        """Return info dict about the CSF file."""
        return {
            'type': 'CSF',
            'magic': 'CESF',
            'file_size': len(self.raw_data),
            'block_count': self.block_count,
            'compressed_size': self.compressed_size,
            'decompressed_size': self.decompressed_size,
            'block_size': self.block_size,
            'compression_ratio': f"{self.compressed_size / self.decompressed_size:.2%}" if self.decompressed_size else "N/A",
            'checksum': f"0x{self.checksum:08X}",
        }


def compress_to_csf(esf_data, block_size=CSF_BLOCK_SIZE):
    """Compress ESF data to CSF format."""
    blocks = []
    pos = 0
    while pos < len(esf_data):
        chunk = esf_data[pos:pos + block_size]
        compressed = zlib.compress(chunk, 9)  # Best compression
        blocks.append((compressed, len(chunk)))
        pos += block_size

    # Build CSF file
    block_count = len(blocks)
    total_compressed = sum(len(b[0]) for b in blocks)

    # Build header
    header = bytearray(CSF_HEADER_SIZE)
    struct.pack_into('4s', header, 0, CSF_MAGIC)
    struct.pack_into('<I', header, 0x04, block_count)
    struct.pack_into('<I', header, 0x08, total_compressed + (block_count - 1) * 8)
    struct.pack_into('<I', header, 0x10, len(esf_data))
    struct.pack_into('<I', header, 0x18, CSF_HEADER_SIZE)

    # Checksum: CRC32 of decompressed data
    checksum = zlib.crc32(esf_data) & 0xFFFFFFFF
    struct.pack_into('<I', header, 0x20, len(blocks[0][0]) if blocks else 0)
    struct.pack_into('<I', header, 0x24, checksum)
    struct.pack_into('<I', header, 0x28, len(blocks[0][0]) if blocks else 0)
    struct.pack_into('<I', header, 0x2C, block_size)

    # Build data section
    result = bytearray(header)
    for i, (compressed, decompressed_len) in enumerate(blocks):
        result.extend(compressed)
        if i < block_count - 1:
            # Inter-block header
            next_csize = len(blocks[i + 1][0])
            next_dsize = blocks[i + 1][1]
            result.extend(struct.pack('<II', next_csize, next_dsize))

    return bytes(result)


# ============================================================
# ESF (FJBO) Format - Recursive Typed Tree
# ============================================================
# File layout:
#   [32-byte header] [root node] [child nodes recursively...]
#
# Header (32 bytes / 0x20):
#   0x00: "FJBO" magic (4 bytes)
#   0x04: uint32 version (1=model/zone, 2+=special formats)
#   0x08: uint32 constant (0xAB4F = 43855)
#   0x0C: uint32 reserved (0)
#   0x10: uint32 header_size (0x20 = 32)
#   0x14: uint32 reserved (0)
#   0x18: uint64 padding (0xFFFFFFFFFFFFFFFF)
#
# Node format (12 bytes each):
#   0x00: uint32 type_id  - format/type identifier for data interpretation
#   0x04: uint32 data_size - bytes of content (children + inline data)
#   0x08: uint32 child_count - number of child nodes (0 = leaf)
#
# Leaf nodes (child_count=0):
#   Followed by data_size bytes of inline data
# Branch nodes (child_count>0):
#   Followed by child_count child nodes (parsed recursively)
#
# Total file size = 0x20 + 12 + root.data_size
#
# Known type_ids:
#   0x0A010 - model container (CHAR, ITEM, AMBTRACK, ITEMICON)
#   0x08100 - zone/scene container (TUNARIA, ARENA, SCENE)
#   0x09000 - global resource block (at file end)
#   0x02C00 - standard model entry (ITEM)
#   0x62700 - standard character model (CHAR)
#   0x22000 - alternate model format
#   0x22200 - complex model with 8 sub-nodes
#   0x02C10 - bounding box (28 bytes: hash + 6 floats)
#   0x01000 - palette/texture data (variable)
#   0x31100 - rendering state (80 bytes)
#   0x21200 - vertex data (variable)
#   0x24200 - mesh topology (variable)
#   0x02D00 - cross-reference (8 bytes)
#   0x02C30 - model properties (100 bytes)
#   0x11111 - model hash/identifier (4 bytes)
#   0x28200 - zone metadata container


class ESFNode:
    """A node in the ESF recursive tree."""

    __slots__ = ['type_id', 'data_size', 'child_count', 'file_pos',
                 'children', 'inline_data', 'depth', 'index']

    def __init__(self, type_id, data_size, child_count, file_pos, depth=0):
        self.type_id = type_id
        self.data_size = data_size
        self.child_count = child_count
        self.file_pos = file_pos
        self.depth = depth
        self.index = -1
        self.children = []
        self.inline_data = None

    @property
    def total_size(self):
        """Total serialized size including 12-byte header."""
        return self.data_size + 12

    @property
    def is_leaf(self):
        return self.child_count == 0

    def find_leaves(self, type_id=None):
        """Find all leaf nodes, optionally filtered by type_id."""
        if self.is_leaf:
            if type_id is None or self.type_id == type_id:
                yield self
        for child in self.children:
            yield from child.find_leaves(type_id)

    def find_by_type(self, type_id):
        """Find all nodes (leaf or branch) with given type_id."""
        if self.type_id == type_id:
            yield self
        for child in self.children:
            yield from child.find_by_type(type_id)

    def get_hash(self):
        """Get model hash from 0x11111-type leaf child, if present."""
        for leaf in self.find_leaves(0x11111):
            if leaf.inline_data and len(leaf.inline_data) >= 4:
                return struct.unpack_from('<I', leaf.inline_data, 0)[0]
        return None

    def get_bbox(self):
        """Get bounding box from 0x2C10-type leaf child."""
        for leaf in self.find_leaves(0x2C10):
            if leaf.inline_data and len(leaf.inline_data) >= 28:
                d = leaf.inline_data
                return {
                    'hash': struct.unpack_from('<I', d, 0)[0],
                    'min': struct.unpack_from('<3f', d, 4),
                    'max': struct.unpack_from('<3f', d, 16),
                }
        # Also check 0x42710 type (CHAR)
        for leaf in self.find_leaves(0x42710):
            if leaf.inline_data and len(leaf.inline_data) >= 28:
                d = leaf.inline_data
                return {
                    'hash': struct.unpack_from('<I', d, 0)[0],
                    'min': struct.unpack_from('<3f', d, 4),
                    'max': struct.unpack_from('<3f', d, 16),
                }
        return None

    def tree_summary(self, max_depth=3, indent=0):
        """Return tree structure summary string."""
        lines = []
        prefix = '  ' * indent
        type_str = f'0x{self.type_id:05X}'
        if self.is_leaf:
            lines.append(f'{prefix}[{type_str}] leaf {self.data_size}B @0x{self.file_pos:X}')
        else:
            lines.append(f'{prefix}[{type_str}] branch cnt={self.child_count} '
                         f'size={self.data_size}B @0x{self.file_pos:X}')
            if indent < max_depth:
                for child in self.children:
                    lines.extend(child.tree_summary(max_depth, indent + 1).splitlines())
            elif self.children:
                lines.append(f'{prefix}  ... ({len(self.children)} children)')
        return '\n'.join(lines)


class ESFFile:
    """Parser for FJBO (ESF) game asset files with full tree parsing."""

    def __init__(self, filepath):
        self.filepath = filepath
        self.file_size = os.path.getsize(filepath)
        with open(filepath, 'rb') as f:
            self.data = f.read()
        self._parse_header()
        self._parse_tree()

    def _parse_header(self):
        if self.data[:4] != ESF_MAGIC:
            raise ValueError("Not an ESF file (expected FJBO magic)")

        self.version = struct.unpack_from('<I', self.data, 0x04)[0]
        self.constant = struct.unpack_from('<I', self.data, 0x08)[0]
        self.header_size = struct.unpack_from('<I', self.data, 0x10)[0]

    def _parse_node(self, pos, depth=0, max_depth=20):
        """Recursively parse a tree node at the given file position."""
        if pos + 12 > len(self.data) or depth > max_depth:
            return None, pos

        type_id = struct.unpack_from('<I', self.data, pos)[0]
        data_size = struct.unpack_from('<I', self.data, pos + 4)[0]
        child_count = struct.unpack_from('<I', self.data, pos + 8)[0]

        if data_size > len(self.data) or child_count > 100000:
            return None, pos + 12

        node = ESFNode(type_id, data_size, child_count, pos, depth)
        pos += 12

        if child_count == 0:
            node.inline_data = self.data[pos:pos + data_size]
            pos += data_size
        else:
            for i in range(child_count):
                child, pos = self._parse_node(pos, depth + 1, max_depth)
                if child is not None:
                    child.index = i
                    node.children.append(child)

        return node, pos

    def _parse_tree(self):
        """Parse the full tree structure starting at offset 0x20."""
        self.root, end_pos = self._parse_node(0x20, depth=0)
        self.tree_complete = (end_pos == len(self.data)) if self.root else False

    def get_model_container(self):
        """Find the main model/entry container node (first branch child of root)."""
        if not self.root or not self.root.children:
            return None
        # The model container is typically the first child with many sub-entries
        for child in self.root.children:
            if child.child_count > 0:
                return child
        return None

    def get_models(self):
        """Get list of model entry nodes (depth-2 children)."""
        container = self.get_model_container()
        if container:
            return container.children
        return []

    def get_model_count(self):
        """Get number of models/entries."""
        container = self.get_model_container()
        return container.child_count if container else 0

    def extract_model(self, index):
        """Extract raw bytes for model at given index."""
        models = self.get_models()
        if index < 0 or index >= len(models):
            raise IndexError(f"Model index {index} out of range (0-{len(models)-1})")
        model = models[index]
        start = model.file_pos
        end = start + model.total_size
        return self.data[start:end]

    def info(self):
        """Return info dict about the ESF file."""
        info = {
            'type': 'ESF',
            'magic': 'FJBO',
            'file_size': self.file_size,
            'version': self.version,
            'constant': f'0x{self.constant:04X}',
            'header_size': self.header_size,
            'tree_complete': self.tree_complete,
        }

        if self.root:
            info['root_type'] = f'0x{self.root.type_id:05X}'
            info['root_children'] = self.root.child_count
            info['total_nodes'] = self._count_nodes(self.root)
            info['model_count'] = self.get_model_count()

            # Model type distribution
            models = self.get_models()
            if models:
                from collections import Counter
                type_dist = Counter(m.type_id for m in models)
                info['model_types'] = {f'0x{t:05X}': c for t, c in type_dist.most_common()}

        return info

    def _count_nodes(self, node):
        """Count total nodes in subtree."""
        count = 1
        for child in node.children:
            count += self._count_nodes(child)
        return count

    def get_md5(self):
        """Get MD5 hash of full file."""
        return hashlib.md5(self.data).hexdigest()

    def dump_header(self, num_bytes=256):
        """Return hex dump of header area."""
        lines = []
        data = self.data[:min(num_bytes, len(self.data))]
        for i in range(0, len(data), 16):
            hex_part = ' '.join(f'{b:02X}' for b in data[i:i+16])
            ascii_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data[i:i+16])
            lines.append(f'  {i:04X}: {hex_part:<48} {ascii_part}')
        return '\n'.join(lines)


def compare_esf_files(file1, file2):
    """Compare two ESF files and report differences."""
    esf1 = ESFFile(file1)
    esf2 = ESFFile(file2)

    print(f"\n{'='*60}")
    print(f"Comparing: {os.path.basename(file1)} vs {os.path.basename(file2)}")
    print(f"{'='*60}")

    # Size comparison
    print(f"\nFile sizes: {esf1.file_size:,} vs {esf2.file_size:,} bytes")
    if esf1.file_size != esf2.file_size:
        print(f"  Difference: {abs(esf1.file_size - esf2.file_size):,} bytes")

    # Version comparison
    print(f"Versions: {esf1.version} vs {esf2.version}")

    # Read both files fully for byte-level comparison
    with open(file1, 'rb') as f:
        data1 = f.read()
    with open(file2, 'rb') as f:
        data2 = f.read()

    # Find first difference
    min_len = min(len(data1), len(data2))
    first_diff = None
    diff_count = 0
    diff_regions = []
    in_diff = False
    diff_start = None

    for i in range(min_len):
        if data1[i] != data2[i]:
            diff_count += 1
            if not in_diff:
                diff_start = i
                in_diff = True
            if first_diff is None:
                first_diff = i
        else:
            if in_diff:
                diff_regions.append((diff_start, i))
                in_diff = False

    if in_diff:
        diff_regions.append((diff_start, min_len))

    if first_diff is not None:
        print(f"\nFirst difference at offset 0x{first_diff:X}")
        print(f"Total differing bytes: {diff_count:,}")
        print(f"Difference regions: {len(diff_regions)}")

        # Show first few difference regions
        for start, end in diff_regions[:10]:
            size = end - start
            print(f"\n  Region 0x{start:06X} - 0x{end:06X} ({size} bytes)")
            # Show a few bytes from each
            show = min(32, size)
            hex1 = ' '.join(f'{data1[start+j]:02X}' for j in range(show))
            hex2 = ' '.join(f'{data2[start+j]:02X}' for j in range(show))
            print(f"    File1: {hex1}")
            print(f"    File2: {hex2}")

        if len(diff_regions) > 10:
            print(f"\n  ... and {len(diff_regions) - 10} more regions")
    else:
        print("\nFiles are IDENTICAL in overlapping region")

    if len(data1) != len(data2):
        print(f"\nExtra bytes in {'file1' if len(data1) > len(data2) else 'file2'}: "
              f"{abs(len(data1) - len(data2)):,}")


def extract_all_files(iso_dir, output_dir):
    """Extract all ESF/CSF files from mounted ISO directory."""
    iso_path = Path(iso_dir)
    out_path = Path(output_dir)

    data_dir = iso_path / 'DATA'
    data2_dir = iso_path / 'DATA2'

    # Create output directories
    (out_path / 'DATA').mkdir(parents=True, exist_ok=True)
    (out_path / 'DATA2').mkdir(parents=True, exist_ok=True)
    (out_path / 'DECOMPRESSED').mkdir(parents=True, exist_ok=True)

    # Copy ESF files from DATA
    if data_dir.exists():
        for f in data_dir.iterdir():
            if f.suffix.upper() in ['.ESF', '.TXT', '.INI', '.XML', '.DAT']:
                dest = out_path / 'DATA' / f.name
                print(f"Copying {f.name} ({f.stat().st_size:,} bytes)")
                with open(f, 'rb') as src, open(dest, 'wb') as dst:
                    while True:
                        chunk = src.read(1024 * 1024)
                        if not chunk:
                            break
                        dst.write(chunk)

    # Copy and decompress CSF files from DATA2
    if data2_dir.exists():
        for f in data2_dir.iterdir():
            if f.suffix.upper() in ['.CSF', '.ESF']:
                dest = out_path / 'DATA2' / f.name
                print(f"Copying {f.name} ({f.stat().st_size:,} bytes)")
                with open(f, 'rb') as src, open(dest, 'wb') as dst:
                    while True:
                        chunk = src.read(1024 * 1024)
                        if not chunk:
                            break
                        dst.write(chunk)

                # Decompress CSF files
                if f.suffix.upper() == '.CSF':
                    try:
                        csf = CSFFile(str(f))
                        esf_name = f.stem + '.ESF'
                        esf_dest = out_path / 'DECOMPRESSED' / esf_name
                        print(f"  Decompressing {f.name} -> {esf_name}")
                        esf_data = csf.decompress()
                        with open(esf_dest, 'wb') as dst:
                            dst.write(esf_data)
                        print(f"  Decompressed: {len(esf_data):,} bytes")
                    except Exception as e:
                        print(f"  Decompression failed: {e}")

    print(f"\nExtraction complete -> {output_dir}")


def print_info(filepath):
    """Print detailed info about an ESF or CSF file."""
    with open(filepath, 'rb') as f:
        magic = f.read(4)

    print(f"\n{'='*50}")
    print(f"File: {os.path.basename(filepath)}")
    print(f"{'='*50}")

    if magic == CSF_MAGIC:
        csf = CSFFile(filepath)
        info = csf.info()
        for k, v in info.items():
            print(f"  {k}: {v}")
    elif magic == ESF_MAGIC:
        esf = ESFFile(filepath)
        info = esf.info()
        for k, v in info.items():
            print(f"  {k}: {v}")
        if esf.tree_complete:
            print(f"\n  Tree structure (depth 0-2):")
            if esf.root:
                print(esf.root.tree_summary(max_depth=2, indent=2))
        print(f"\n  MD5: {esf.get_md5()}")
    else:
        print(f"  Unknown file type: {magic}")


def print_tree(filepath, max_depth=4):
    """Print full tree structure of an ESF file."""
    esf = ESFFile(filepath)
    print(f"\n{os.path.basename(filepath)} - Tree Structure")
    print(f"{'='*60}")
    print(f"Version: {esf.version}, Nodes: {esf._count_nodes(esf.root)}, "
          f"Complete: {esf.tree_complete}")
    print()
    if esf.root:
        print(esf.root.tree_summary(max_depth=max_depth))


def print_models(filepath):
    """Print model index for an ESF file."""
    esf = ESFFile(filepath)
    models = esf.get_models()
    if not models:
        print(f"No models found in {os.path.basename(filepath)}")
        return

    print(f"\n{os.path.basename(filepath)} - {len(models)} Models")
    print(f"{'='*70}")
    print(f"{'Idx':>4} {'Type':>7} {'Size':>8} {'FilePos':>10} {'Hash':>10} {'SubNodes':>8}")
    print(f"{'-'*4:>4} {'-'*7:>7} {'-'*8:>8} {'-'*10:>10} {'-'*10:>10} {'-'*8:>8}")

    for i, model in enumerate(models):
        model_hash = model.get_hash()
        hash_str = f'0x{model_hash:08X}' if model_hash else '-'
        sub_count = sum(1 for _ in model.find_leaves())
        print(f'{i:4d} 0x{model.type_id:05X} {model.total_size:8,} 0x{model.file_pos:08X} '
              f'{hash_str:>10} {sub_count:8d}')

    # Summary
    from collections import Counter
    type_dist = Counter(m.type_id for m in models)
    sizes = [m.total_size for m in models]
    print(f"\nSummary:")
    print(f"  Total models: {len(models)}")
    print(f"  Size range: {min(sizes):,} - {max(sizes):,} bytes (avg {sum(sizes)//len(sizes):,})")
    print(f"  Types: {', '.join(f'0x{t:05X}={c}' for t, c in type_dist.most_common())}")


def hex_dump(filepath, offset=0, length=256):
    """Print hex dump of a file."""
    with open(filepath, 'rb') as f:
        f.seek(offset)
        data = f.read(length)

    print(f"\n{os.path.basename(filepath)} @ 0x{offset:X} ({length} bytes):")
    for i in range(0, len(data), 16):
        hex_part = ' '.join(f'{b:02X}' for b in data[i:i+16])
        ascii_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data[i:i+16])
        print(f'  {offset+i:06X}: {hex_part:<48} {ascii_part}')


# ============================================================
# Main CLI
# ============================================================
def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return

    cmd = sys.argv[1].lower()

    if cmd == 'info' and len(sys.argv) >= 3:
        print_info(sys.argv[2])

    elif cmd == 'tree' and len(sys.argv) >= 3:
        max_depth = int(sys.argv[3]) if len(sys.argv) > 3 else 4
        print_tree(sys.argv[2], max_depth)

    elif cmd == 'models' and len(sys.argv) >= 3:
        print_models(sys.argv[2])

    elif cmd == 'extract_model' and len(sys.argv) >= 5:
        esf = ESFFile(sys.argv[2])
        idx = int(sys.argv[3])
        model_data = esf.extract_model(idx)
        with open(sys.argv[4], 'wb') as f:
            f.write(model_data)
        print(f"Extracted model {idx} ({len(model_data):,} bytes) -> {sys.argv[4]}")

    elif cmd == 'decompress' and len(sys.argv) >= 3:
        src = sys.argv[2]
        dst = sys.argv[3] if len(sys.argv) > 3 else src.rsplit('.', 1)[0] + '.ESF'
        csf = CSFFile(src)
        data = csf.decompress()
        with open(dst, 'wb') as f:
            f.write(data)
        print(f"Decompressed {os.path.basename(src)} -> {os.path.basename(dst)} ({len(data):,} bytes)")

    elif cmd == 'compress' and len(sys.argv) >= 3:
        src = sys.argv[2]
        dst = sys.argv[3] if len(sys.argv) > 3 else src.rsplit('.', 1)[0] + '.CSF'
        with open(src, 'rb') as f:
            data = f.read()
        csf_data = compress_to_csf(data)
        with open(dst, 'wb') as f:
            f.write(csf_data)
        print(f"Compressed {os.path.basename(src)} -> {os.path.basename(dst)} ({len(csf_data):,} bytes)")

    elif cmd == 'compare' and len(sys.argv) >= 4:
        compare_esf_files(sys.argv[2], sys.argv[3])

    elif cmd == 'extract_all' and len(sys.argv) >= 4:
        extract_all_files(sys.argv[2], sys.argv[3])

    elif cmd == 'dump' and len(sys.argv) >= 3:
        offset = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0
        length = int(sys.argv[4], 0) if len(sys.argv) > 4 else 256
        hex_dump(sys.argv[2], offset, length)

    else:
        print(__doc__)


if __name__ == '__main__':
    main()
