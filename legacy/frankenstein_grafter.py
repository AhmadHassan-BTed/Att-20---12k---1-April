# #!/usr/bin/env python3
# """
# EQOA Surgical Format Grafter (Frankenstein Patcher)
# ===================================================
# A production-grade, layout-aware surgical binary splicing patcher.
# Transforms vanilla EQOA character model payloads into natively compatible 
# EverQuest Online Adventures: Frontiers character model assets by updating 
# the node tree structure to align exactly with the Frontiers engine's expectations.

# Surgical Transformations:
# 1. Root type_id upgrade: 0x62700 -> 0x72700.
# 2. Bone/Skeleton transform node type_id upgrade: 0x12400 -> 0x22400.
# 3. Root child list expansion: appends Frontiers-specific nodes 0x02950 (size 0) 
#    and 0x02960 (size 4, 0x00000000) to match the 17-child format layout.
# 4. Recursive data size recalculation across the entire tree.
# """

# import os
# import sys
# import struct
# import glob


# def parse_node(data, pos):
#     """Recursively parse a binary node tree."""
#     if pos + 12 > len(data):
#         return None, pos
        
#     type_id = struct.unpack_from('<I', data, pos)[0]
#     data_size = struct.unpack_from('<I', data, pos + 4)[0]
#     child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
#     node = {
#         'type_id': type_id,
#         'data_size': data_size,
#         'child_count': child_count,
#         'children': [],
#         'inline_data': None
#     }
    
#     next_pos = pos + 12
#     if child_count == 0:
#         if next_pos + data_size > len(data):
#             raise EOFError(f"Unexpected EOF reading leaf node at 0x{next_pos:X} (expected {data_size} bytes)")
#         node['inline_data'] = data[next_pos : next_pos + data_size]
#         next_pos += data_size
#     else:
#         for _ in range(child_count):
#             child, next_pos = parse_node(data, next_pos)
#             if child is not None:
#                 node['children'].append(child)
                
#     return node, next_pos


# def update_node_sizes(node):
#     """Recursively calculate and update correct data_size for every node in the tree."""
#     if node['child_count'] == 0:
#         node['data_size'] = len(node['inline_data'])
#     else:
#         size = 0
#         for child in node['children']:
#             update_node_sizes(child)
#             size += 12 + child['data_size']
#         node['data_size'] = size


# def serialize_node(node):
#     """Recursively serialize a node tree to binary bytes."""
#     data = bytearray()
#     header = struct.pack('<III', node['type_id'], node['data_size'], node['child_count'])
#     data.extend(header)
    
#     if node['child_count'] == 0:
#         if node['inline_data'] is not None:
#             data.extend(node['inline_data'])
#     else:
#         for child in node['children']:
#             data.extend(serialize_node(child))
            
#     return bytes(data)


# def patch_payload(filepath):
#     """Surgically parse, update structural nodes, resize, and serialize a payload file."""
#     filename = os.path.basename(filepath)
    
#     with open(filepath, 'rb') as f:
#         data = f.read()
        
#     # Check if this is a master model payload
#     if len(data) < 12:
#         return False
        
#     type_id = struct.unpack_from('<I', data, 0)[0]
#     if type_id not in (0x62700, 0x72700):
#         # Skip texture or dependency sub-assets which do not start with character model root types
#         return False
        
#     try:
#         root, _ = parse_node(data, 0)
#     except Exception as e:
#         print(f"  [-] Failed to parse node tree of {filename}: {e}")
#         return False
        
#     was_patched = False
    
#     # 1. Root type_id upgrade
#     if root['type_id'] == 0x62700:
#         root['type_id'] = 0x72700
#         was_patched = True
        
#     # 2. Upgrade Bone/Skeleton transform node type_id: 0x12400 -> 0x22400
#     for child in root['children']:
#         if child['type_id'] == 0x12400:
#             child['type_id'] = 0x22400
#             was_patched = True
            
#     # 3. Add Frontiers-specific trailer child nodes (0x02950 and 0x02960) to expand 15 -> 17 children
#     if len(root['children']) == 15:
#         # Check current child types to be absolutely safe
#         child_types = [c['type_id'] for c in root['children']]
#         if child_types[-1] == 0x02940:
#             # Create Child 15: type 0x02950, size 0
#             child15 = {
#                 'type_id': 0x02950,
#                 'data_size': 0,
#                 'child_count': 0,
#                 'children': [],
#                 'inline_data': b''
#             }
#             # Create Child 16: type 0x02960, size 4
#             child16 = {
#                 'type_id': 0x02960,
#                 'data_size': 4,
#                 'child_count': 0,
#                 'children': [],
#                 'inline_data': b'\x00\x00\x00\x00'
#             }
#             root['children'].append(child15)
#             root['children'].append(child16)
#             root['child_count'] = 17
#             was_patched = True
            
#     if was_patched:
#         # 4. Recalculate sizes recursively
#         update_node_sizes(root)
        
#         # 5. Serialize and overwrite payload file
#         patched_data = serialize_node(root)
#         with open(filepath, 'wb') as f:
#             f.write(patched_data)
            
#         print(f"  [+] Surgically Grafted {filename}:")
#         print(f"      - Root Node: 0x62700 -> 0x72700")
#         print(f"      - Children Count: 15 -> 17 (Added 0x02950 and 0x02960)")
#         print(f"      - Skeleton Transforms Node: 0x12400 -> 0x22400")
#         print(f"      - Original Size: {len(data):,} bytes | New Size: {len(patched_data):,} bytes")
#         return True
#     else:
#         return False


# def main():
#     payload_dir = './workspace/payloads'
#     print("[*] Commencing Surgical Frontiers Engine Formatting...")
    
#     bin_files = sorted(glob.glob(os.path.join(payload_dir, '*.bin')))
#     if not bin_files:
#         print("[-] Error: No payloads found in workspace/payloads directory.")
#         sys.exit(1)
        
#     grafted_count = 0
#     for filepath in bin_files:
#         success = patch_payload(filepath)
#         if success:
#             grafted_count += 1
            
#     print(f"\n[+] FORMATTING COMPLETE: {grafted_count} master payloads successfully updated for the Frontiers engine!")


# if __name__ == '__main__':
#     main()

#!/usr/bin/env python3
"""
frankenstein_grafter.py  —  EQOA Frontiers / Vanilla Binary Splicer
=====================================================================
Performs a precision binary graft: transplants the native Frontiers
structural header (which contains the engine-required memory layout,
material flags, and VU1 microprogram pointers) onto a Vanilla geometry
payload (which contains the actual mesh DMA/VIF packets).

Architecture overview
---------------------
  [frontiers_donor.bin]    [vanilla_target.bin]
  ┌──────────────────┐     ┌──────────────────┐
  │  Frontiers HDR   │     │   Vanilla HDR    │  ← discarded
  │  (material/ptr)  │     │  (incompatible)  │
  ├──────────────────┤     ├──────────────────┤
  │  Frontiers MESH  │     │  Vanilla MESH    │  ← kept
  │  (discarded)     │     │  (VIF UNPACK     │
  └──────────────────┘     │   DMA geometry)  │
                           └──────────────────┘
  Result:
  ┌──────────────────┐
  │  Frontiers HDR   │
  ├──────────────────┤
  │  Vanilla MESH    │
  └──────────────────┘

The script also patches any 32-bit total-size field(s) found in the
donor header so the Frontiers engine does not truncate the grafted body.

Usage
-----
  # Single pair (development / testing mode):
  python frankenstein_grafter.py \
      --donor  workspace/payloads/frontiers_donor.bin \
      --target workspace/payloads/vanilla_target.bin  \
      --output workspace/grafted/grafted_out.bin

  # Batch mode — graft every Vanilla payload that has a matching donor:
  python frankenstein_grafter.py \
      --batch  \
      --donor-dir  workspace/payloads/frontiers \
      --target-dir workspace/payloads/vanilla    \
      --output-dir workspace/grafted

Dependencies: Python 3.8+ stdlib only (struct, os, sys, argparse, glob, shutil).
No third-party packages required.
"""

import os
import sys
import glob
import struct
import shutil
import argparse


# ─────────────────────────────────────────────────────────────────────────────
# Constants
# ─────────────────────────────────────────────────────────────────────────────

# The VIFcode opcode occupies the top byte of a 32-bit little-endian word.
# UNPACK commands: MSB in range [0x60, 0x7F]  →  raw mesh data upload to VU1 mem
# STCYCL command:  MSB == 0x01 in VIF encoding, but the full 32-bit pattern
#                  seen in EQOA payloads is 0x01xxxxxx (cycle/write-cycle regs).
# NOP / MARK / FLUSH etc. have predictable MSBs; we intentionally exclude them
# here because we want the *first genuine geometry upload*, not a control code.

VIF_UNPACK_MSB_MIN = 0x60  # inclusive
VIF_UNPACK_MSB_MAX = 0x7F  # inclusive
VIF_STCYCL_MSB     = 0x01  # 0x01xxxxxx  — commonly precedes an UNPACK burst

# Minimum number of *consecutive* matching DWORD candidates that must be found
# before we accept an offset as the true mesh-data boundary.
# A single accidental DWORD match inside a header is common; three in a row
# is a strong signal that we have entered the VIF packet stream.
CONSECUTIVE_VIF_THRESHOLD = 3

# Candidate offsets for the 32-bit total-size field inside the header.
# The Frontiers engine typically stores the payload size at one (sometimes
# several) of these well-known header offsets.  All are relative to byte 0 of
# the *individual* asset .bin (i.e., relative to the node body, not the ESF).
SIZE_FIELD_PROBE_OFFSETS = [0x04, 0x08, 0x0C, 0x10, 0x14]

# Minimum plausible payload size (bytes).  Any header field whose current value
# is smaller than this is almost certainly NOT a size field and will be skipped.
MIN_PLAUSIBLE_PAYLOAD_SIZE = 64

# Maximum plausible payload size (bytes) — 8 MB; no single EQOA model is larger.
MAX_PLAUSIBLE_PAYLOAD_SIZE = 8 * 1024 * 1024


# ─────────────────────────────────────────────────────────────────────────────
# Step 1 — Dynamic mesh-start scanner
# ─────────────────────────────────────────────────────────────────────────────

def find_mesh_start(data: bytes, label: str) -> int:
    """
    Scan *data* forward in 4-byte aligned steps and return the byte offset of
    the first position that looks like the beginning of a VIF/DMA packet burst.

    Detection strategy
    ------------------
    We read each aligned DWORD and test its most-significant byte (MSB):
      • UNPACK: MSB ∈ [0x60, 0x7F]  — direct mesh upload instruction
      • STCYCL: MSB == 0x01         — cycle register write; always precedes UNPACKs

    To avoid false-positives on sparse header constants, we require
    CONSECUTIVE_VIF_THRESHOLD consecutive matching DWORDs before we commit.

    Parameters
    ----------
    data  : raw bytes of the .bin file
    label : human-readable name used in diagnostic output

    Returns
    -------
    int — byte offset of the first DWORD in the confirmed VIF burst, or -1 if
          no burst is found (caller must treat this as a fatal error).
    """
    file_len   = len(data)
    run_start  = -1    # offset where the current candidate run began
    run_length = 0     # how many consecutive VIF-like DWORDs we have seen

    for pos in range(0, file_len - 4, 4):
        dword = struct.unpack_from('<I', data, pos)[0]
        msb   = (dword >> 24) & 0xFF

        is_vif = (VIF_UNPACK_MSB_MIN <= msb <= VIF_UNPACK_MSB_MAX) or \
                 (msb == VIF_STCYCL_MSB)

        if is_vif:
            if run_length == 0:
                run_start = pos          # begin a new candidate run
            run_length += 1
            if run_length >= CONSECUTIVE_VIF_THRESHOLD:
                # Confirmed: we have found the mesh region.
                # Back up to where the run started, not where the threshold fired.
                print(f"  [+] {label}: mesh-start detected at offset "
                      f"0x{run_start:08X} "
                      f"(confirmed after {run_length} consecutive VIF DWORDs, "
                      f"threshold fired at 0x{pos:08X})")
                return run_start
        else:
            # Reset the run — this DWORD is not a VIF instruction.
            run_start  = -1
            run_length = 0

    # Nothing found in the entire file.
    print(f"  [-] {label}: no VIF/DMA mesh start found — file may be a pure "
          f"header/stub, or VIF packets are not 4-byte aligned as expected.")
    return -1


# ─────────────────────────────────────────────────────────────────────────────
# Step 2 — Header size-field detector and patcher
# ─────────────────────────────────────────────────────────────────────────────

def patch_size_fields(header_blob: bytearray,
                      new_total_size: int,
                      original_donor_size: int) -> list[int]:
    """
    Probe SIZE_FIELD_PROBE_OFFSETS inside *header_blob* and overwrite any
    DWORD whose current value is "close" to the original donor file's total
    size — indicating it is a size field — with *new_total_size*.

    "Close" is defined generously: within ±512 bytes of original_donor_size.
    This handles the common case where the stored size is the body-only length
    (i.e., excludes the 12-byte ESF node header that wraps the asset).

    Parameters
    ----------
    header_blob        : mutable bytearray of the Frontiers header region
    new_total_size     : byte length of the fully grafted payload
    original_donor_size: byte length of the original frontiers_donor.bin

    Returns
    -------
    list[int] — offsets (relative to header start) that were patched
    """
    patched_offsets = []
    tolerance       = 512   # bytes; see docstring

    for off in SIZE_FIELD_PROBE_OFFSETS:
        # Guard: do not read past the header boundary
        if off + 4 > len(header_blob):
            continue

        current_val = struct.unpack_from('<I', header_blob, off)[0]

        # Heuristic gate 1: value must be in a plausible size range
        if not (MIN_PLAUSIBLE_PAYLOAD_SIZE <= current_val <= MAX_PLAUSIBLE_PAYLOAD_SIZE):
            continue

        # Heuristic gate 2: value must be near the original donor's file size
        if abs(int(current_val) - original_donor_size) <= tolerance:
            old_val = current_val
            struct.pack_into('<I', header_blob, off, new_total_size)
            patched_offsets.append(off)
            print(f"    [patch] offset 0x{off:02X}: 0x{old_val:08X} "
                  f"({old_val:,} B)  →  0x{new_total_size:08X} "
                  f"({new_total_size:,} B)")

    if not patched_offsets:
        print(f"    [warn] No size field auto-detected at standard offsets "
              f"{[hex(o) for o in SIZE_FIELD_PROBE_OFFSETS]}. "
              f"The engine may use an alternative layout; verify with Phase 4 diff.")

    return patched_offsets


# ─────────────────────────────────────────────────────────────────────────────
# Step 3 — Core graft logic
# ─────────────────────────────────────────────────────────────────────────────

def graft(donor_path: str,
          target_path: str,
          output_path: str,
          dry_run: bool = False) -> bool:
    """
    Perform the Frankenstein graft for a single donor / target pair.

    Procedure
    ---------
    1.  Load donor (Frontiers native .bin) and target (Vanilla .bin).
    2.  Scan donor  → frontiers_mesh_start  (where its own geometry begins)
    3.  Scan target → vanilla_mesh_start    (where Vanilla geometry begins)
    4.  Splice:  new_payload = donor[0 : frontiers_mesh_start]
                             + target[vanilla_mesh_start : EOF]
    5.  Patch any 32-bit size field in the donor header to reflect new length.
    6.  Write the result to output_path.

    Parameters
    ----------
    donor_path  : path to a native Frontiers asset .bin (header donor)
    target_path : path to a Vanilla asset .bin (geometry donor)
    output_path : destination path for the grafted .bin
    dry_run     : if True, scan and report but do not write output

    Returns
    -------
    bool — True on success, False on any non-fatal error.
    """
    print(f"\n{'─'*60}")
    print(f"  DONOR  : {donor_path}")
    print(f"  TARGET : {target_path}")
    print(f"  OUTPUT : {output_path}")
    print(f"{'─'*60}")

    # ── Load files ──────────────────────────────────────────────────────────
    if not os.path.exists(donor_path):
        print(f"  [-] ABORT: donor file not found: {donor_path}")
        return False
    if not os.path.exists(target_path):
        print(f"  [-] ABORT: target file not found: {target_path}")
        return False

    with open(donor_path, 'rb') as fh:
        frontiers_data = fh.read()
    with open(target_path, 'rb') as fh:
        vanilla_data = fh.read()

    print(f"  Donor  size : {len(frontiers_data):>10,} bytes  (0x{len(frontiers_data):08X})")
    print(f"  Target size : {len(vanilla_data):>10,} bytes  (0x{len(vanilla_data):08X})")

    # ── Step 1: Locate the mesh boundary in both files ───────────────────────
    print(f"\n  [1/4] Scanning donor for Frontiers mesh boundary...")
    frontiers_mesh_start = find_mesh_start(frontiers_data, "donor (Frontiers)")

    print(f"  [1/4] Scanning target for Vanilla mesh boundary...")
    vanilla_mesh_start   = find_mesh_start(vanilla_data,   "target (Vanilla)")

    # Both boundaries must be found for the graft to proceed
    if frontiers_mesh_start == -1:
        print(f"  [-] ABORT: Could not find mesh boundary in donor file.")
        print(f"       This donor may be a pure header stub with no embedded geometry,")
        print(f"       or the VIF packet stream is not 4-byte aligned. Inspect with")
        print(f"       hex_analyzer.py Phase 1 before retrying.")
        return False

    if vanilla_mesh_start == -1:
        print(f"  [-] ABORT: Could not find mesh boundary in Vanilla target.")
        print(f"       Confirm hex_analyzer.py Phase 1 passes for this payload.")
        return False

    header_size   = frontiers_mesh_start            # bytes we take from donor
    geometry_size = len(vanilla_data) - vanilla_mesh_start  # bytes we take from target

    print(f"\n  Header region  (from donor) : bytes [0x000000 : 0x{frontiers_mesh_start:06X}]  "
          f"= {header_size:,} bytes")
    print(f"  Geometry region (from target): bytes [0x{vanilla_mesh_start:06X} : EOF]  "
          f"= {geometry_size:,} bytes")

    # ── Step 2: Splice ───────────────────────────────────────────────────────
    print(f"\n  [2/4] Splicing header + geometry...")
    #
    # We intentionally build the initial splice as an immutable bytes object for
    # correctness verification, then convert the header slice to a bytearray for
    # in-place patching in step 3.
    #
    header_bytes   = bytearray(frontiers_data[0 : frontiers_mesh_start])
    geometry_bytes = vanilla_data[vanilla_mesh_start:]        # immutable slice; no copy needed

    # Pre-patch total size (before merging) so the arithmetic is clear
    new_total_size = len(header_bytes) + len(geometry_bytes)
    print(f"  Grafted payload size : {new_total_size:,} bytes  (0x{new_total_size:08X})")

    # Sanity: grafted payload should always be smaller than the sum of both
    # input files (we discard both headers/geometries that we do not use).
    expected_upper = len(frontiers_data) + len(vanilla_data)
    if new_total_size >= expected_upper:
        # This would indicate a scanning bug — mesh starts were detected too late.
        print(f"  [warn] Grafted size ({new_total_size:,}) >= sum of both inputs "
              f"({expected_upper:,}). Mesh boundary detection may have failed "
              f"silently. Verify offsets before injecting into the ESF.")

    # ── Step 3: Patch size field(s) in the donor header ──────────────────────
    print(f"\n  [3/4] Probing and patching size field(s) in donor header...")
    patched = patch_size_fields(
        header_blob         = header_bytes,
        new_total_size      = new_total_size,
        original_donor_size = len(frontiers_data)
    )

    # ── Step 4: Assemble final blob and write ────────────────────────────────
    if dry_run:
        print(f"\n  [4/4] DRY-RUN — skipping write to {output_path}")
        print(f"  Would write {new_total_size:,} bytes  "
              f"(header={header_size}, geometry={geometry_size}, "
              f"size_fields_patched={len(patched)})")
        return True

    print(f"\n  [4/4] Writing grafted payload to {output_path}...")
    os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)

    with open(output_path, 'wb') as out_fh:
        out_fh.write(header_bytes)    # patched Frontiers header
        out_fh.write(geometry_bytes)  # Vanilla VIF/DMA geometry stream

    # Verify the written file matches our arithmetic
    written_size = os.path.getsize(output_path)
    if written_size != new_total_size:
        print(f"  [-] ERROR: Written file size ({written_size:,}) does not match "
              f"expected size ({new_total_size:,}). Disk write may be incomplete.")
        return False

    print(f"  [+] SUCCESS: {output_path}  ({written_size:,} bytes written)")
    return True


# ─────────────────────────────────────────────────────────────────────────────
# Batch helper — match donors to targets by asset hash in filename
# ─────────────────────────────────────────────────────────────────────────────

def run_batch(donor_dir: str,
              target_dir: str,
              output_dir: str,
              dry_run: bool = False) -> None:
    """
    Iterate over every Vanilla payload in *target_dir* and look for a
    corresponding Frontiers donor in *donor_dir* by matching the asset hash
    embedded in the filename (e.g., "asset_0x05AEBA67.bin").

    Naming convention expected
    --------------------------
      Frontiers donors : <donor_dir>/asset_0x<HASH>.bin
      Vanilla targets  : <target_dir>/asset_0x<HASH>.bin

    If a donor is missing for a particular hash the target is skipped with a
    warning — it is not treated as a fatal error so the rest of the batch
    continues to completion.
    """
    target_files = glob.glob(os.path.join(target_dir, 'asset_0x*.bin'))

    if not target_files:
        print(f"[-] No 'asset_0x*.bin' files found in target directory: {target_dir}")
        sys.exit(1)

    os.makedirs(output_dir, exist_ok=True)

    total     = len(target_files)
    succeeded = 0
    skipped   = 0
    failed    = 0

    print(f"\n[*] Batch graft — {total} Vanilla target(s) found in {target_dir}")

    for target_path in sorted(target_files):
        # Extract the hash token from the filename, e.g. "0x05AEBA67"
        basename = os.path.basename(target_path)
        # Expected format: asset_0x<8 hex digits>.bin
        try:
            hash_token = basename.split('_')[1].split('.')[0]  # "0x05AEBA67"
        except IndexError:
            print(f"  [skip] Unrecognised filename format: {basename}")
            skipped += 1
            continue

        donor_name = f"asset_{hash_token}.bin"
        donor_path = os.path.join(donor_dir, donor_name)
        output_path = os.path.join(output_dir, basename)

        if not os.path.exists(donor_path):
            print(f"\n  [skip] No Frontiers donor for hash {hash_token} "
                  f"(expected: {donor_path})")
            skipped += 1
            continue

        ok = graft(donor_path, target_path, output_path, dry_run=dry_run)
        if ok:
            succeeded += 1
        else:
            failed += 1

    print(f"\n{'='*60}")
    print(f"  BATCH COMPLETE")
    print(f"  Total   : {total}")
    print(f"  Grafted : {succeeded}")
    print(f"  Skipped : {skipped}  (no matching donor)")
    print(f"  Failed  : {failed}")
    print(f"{'='*60}")

    if failed > 0:
        print("\n[-] One or more grafts failed. "
              "Investigate individual errors above before running repack_iso.py.")
        sys.exit(1)


# ─────────────────────────────────────────────────────────────────────────────
# CLI
# ─────────────────────────────────────────────────────────────────────────────

def build_arg_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description="EQOA Frankenstein Grafter — splice Frontiers headers onto Vanilla geometry.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Single graft
  python frankenstein_grafter.py \\
      --donor  workspace/payloads/frontiers/asset_0x05AEBA67.bin \\
      --target workspace/payloads/vanilla/asset_0x05AEBA67.bin  \\
      --output workspace/grafted/asset_0x05AEBA67.bin

  # Dry-run (no output written — just scan and report offsets)
  python frankenstein_grafter.py --dry-run \\
      --donor  workspace/payloads/frontiers/asset_0x05AEBA67.bin \\
      --target workspace/payloads/vanilla/asset_0x05AEBA67.bin  \\
      --output workspace/grafted/asset_0x05AEBA67.bin

  # Batch graft all matching pairs
  python frankenstein_grafter.py --batch \\
      --donor-dir  workspace/payloads/frontiers \\
      --target-dir workspace/payloads/vanilla   \\
      --output-dir workspace/grafted
        """
    )

    # ─── Mode flags ───────────────────────────────────────────────────────────
    mode = p.add_mutually_exclusive_group(required=False)
    mode.add_argument(
        '--batch', action='store_true',
        help='Batch mode: process all matching donor/target pairs by asset hash.'
    )

    # ─── Single-pair arguments ────────────────────────────────────────────────
    p.add_argument('--donor',  metavar='PATH',
                   help='Path to native Frontiers .bin (header donor).')
    p.add_argument('--target', metavar='PATH',
                   help='Path to Vanilla .bin (geometry donor).')
    p.add_argument('--output', metavar='PATH',
                   help='Destination path for grafted .bin.')

    # ─── Batch arguments ──────────────────────────────────────────────────────
    p.add_argument('--donor-dir',  metavar='DIR',
                   help='[batch] Directory containing Frontiers donor .bin files.')
    p.add_argument('--target-dir', metavar='DIR',
                   help='[batch] Directory containing Vanilla target .bin files.')
    p.add_argument('--output-dir', metavar='DIR',
                   help='[batch] Directory to write grafted .bin files.')

    # ─── Universal flags ──────────────────────────────────────────────────────
    p.add_argument('--dry-run', action='store_true',
                   help='Scan and report only; do not write any output files.')

    return p


def main():
    parser = build_arg_parser()
    args   = parser.parse_args()

    print("=" * 60)
    print("  frankenstein_grafter.py  —  EQOA Binary Header Splicer")
    print("=" * 60)

    if args.batch:
        # ── Batch mode ────────────────────────────────────────────────────────
        if not args.donor_dir or not args.target_dir or not args.output_dir:
            parser.error("--batch requires --donor-dir, --target-dir, and --output-dir.")

        run_batch(
            donor_dir  = args.donor_dir,
            target_dir = args.target_dir,
            output_dir = args.output_dir,
            dry_run    = args.dry_run,
        )

    else:
        # ── Single-pair mode ──────────────────────────────────────────────────
        if not args.donor or not args.target or not args.output:
            parser.error("Single-pair mode requires --donor, --target, and --output.")

        ok = graft(
            donor_path  = args.donor,
            target_path = args.target,
            output_path = args.output,
            dry_run     = args.dry_run,
        )

        if not ok:
            print("\n[-] Graft failed. See diagnostic output above.")
            sys.exit(1)

        print(f"\n[+] Graft complete. Feed {args.output} into payload_extractor.py /")
        print(f"    rebuild_esf.py, then rerun repack_iso.py to generate a patched ISO.")


if __name__ == '__main__':
    main()