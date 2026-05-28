#!/usr/bin/env python3
"""
core/manifest_aligner.py
=======================
Rewrites the game's ESF manifest (CHARSEL.ESF / STATION.AUT) so its internal
pointer table points to our sanitized custom geometry nodes instead of the
original ones.

This uses the same ESFParser / ESF rebuilding primitives already present in
this project (core/esf_parser.py, core/esf_rebuilder.py) — which model the
exact same FJBO binary format that DabDavis/eqoa-esf-tools implements.
"""
from __future__ import annotations

import argparse
import os
import struct
import sys
import json
from pathlib import Path

# Bring in the already-parsed ESF primitives from this project
from core.esf_parser import ESFParser, EsfNodeHeader
from core.esf_rebuilder import serialize_node, add_padding_to_tree, EsfHeader

# ----------------------------------------------------------------------
# Public API — the exact logic used to interact with the eqoa-esf-tools
# parsing model is reproduced here using the local ESFParser class which
# implements the identical FJBO format specification.
# ----------------------------------------------------------------------


def load_manifest(manifest_path: str) -> ESFParser:
    """
    Load and parse an ESF manifest file using the FJBO parser.

    This replicates what DabDavis/eqoa-esf-tools does:
    1. Read raw bytes from the manifest ESF.
    2. Instantiate ESFParser and call .parse() to build the node tree.
    3. Return the populated parser so we can traverse the tree.
    """
    if not os.path.exists(manifest_path):
        raise FileNotFoundError(f"Manifest not found: {manifest_path}")

    with open(manifest_path, "rb") as fh:
        data = fh.read()

    parser = ESFParser(data)
    parser.parse()
    return parser


def find_pointer_index(parser: ESFParser, target_asset_hash: int) -> dict | None:
    """
    Traverse the parsed manifest node tree to locate the pointer entry
    that references the asset identified by `target_asset_hash`.

    In the FJBO tree, a model pointer entry is a leaf node whose inline
    data contains the 4-byte asset hash (type 0x11111).  We search the
    entire tree for that hash and return the containing model node so
    we can overwrite its data_size / children.

    Returns the pointer-node dict if found, otherwise None.
    """
    def search(node):
        # A pointer leaf stores the asset hash as its inline_data
        if node["type_id"] == 0x11111 and node["inline_data"]:
            stored = struct.unpack("<I", node["inline_data"][:4])[0]
            if stored == target_asset_hash:
                return node
        for child in node.get("children", []):
            result = search(child)
            if result is not None:
                return result
        return None

    return search(parser.root)


def find_model_container(parser: ESFParser) -> dict:
    """
    Locate the top-level model container node (type 0x0A010).
    This is where our custom sanitized geometry nodes are registered.
    """
    for child in parser.root["children"]:
        if child["type_id"] == 0x0A010:
            return child
    raise ValueError("Model container (0x0A010) not found in manifest")


def align_pointer_to_custom_geometry(
    parser: ESFParser,
    pointer_node: dict,
    custom_node_id: int,
    custom_data_size: int,
) -> None:
    """
    Overwrite the pointer node so it now references our injected
    sanitized geometry node (identified by custom_node_id) instead of
    the original vanilla asset.

    The pointer node lives at a known offset in the tree; we update its
    type_id and data_size in-place so the serializer will emit the new
    values when we rebuild the manifest.
    """
    pointer_node["type_id"] = custom_node_id        # e.g. 0x72700
    pointer_node["data_size"] = custom_data_size   # exact rebuilt size
    print(
        f"  [+] Pointer redirected  -> type=0x{custom_node_id:05X}  "
        f"data_size={custom_data_size}"
    )


def rebuild_and_save(parser: ESFParser, output_path: str) -> None:
    """
    Re-serialize the modified manifest tree and write the aligned
    manifest file using the same EsfHeader / serialize_node primitives
    that the ESF rebuild pipeline uses.

    This is equivalent to calling the DabDavis/eqoa-esf-tools "serialize"
    function on the modified node tree.
    """
    # Preserve original file padding
    integrity = parser.verify_integrity()
    original_padding = integrity["padding_bytes"]

    output_data = bytearray()

    # Build file header
    hdr = dict(
        version=parser.header.version,
        constant=parser.header.constant,
        reserved1=parser.header.reserved1,
        header_size=parser.header.header_size,
        reserved2=parser.header.reserved2,
        padding=parser.header.padding,
    )
    output_data.extend(EsfHeader.build(hdr))

    # Serialize entire tree
    output_data.extend(serialize_node(parser.root))

    # Restore original EOF padding
    if original_padding > 0:
        output_data.extend(b"\x00" * original_padding)

    with open(output_path, "wb") as fh:
        fh.write(output_data)

    print(f"[+] Aligned manifest saved -> {output_path}  ({len(output_data):,} bytes)")


# ----------------------------------------------------------------------
# CLI entry point
# ----------------------------------------------------------------------


def main() -> int:
    parser_cli = argparse.ArgumentParser(
        description=(
            "Align the game manifest (CHARSEL.ESF / STATION.AUT) to point "
            "to sanitized custom geometry using the official FJBO parser."
        )
    )
    parser_cli.add_argument(
        "--manifest",
        type=str,
        required=True,
        help="Path to the source manifest ESF file.",
    )
    parser_cli.add_argument(
        "--original-hash",
        type=str,
        required=True,
        help=(
            "Asset hash (hex, no 0x) of the original geometry node to replace. "
            "e.g. 05AEBA67"
        ),
    )
    parser_cli.add_argument(
        "--custom-node-id",
        type=str,
        required=True,
        help=(
            "Type-ID of the custom sanitized node to point to (hex, no 0x). "
            "e.g. 072700"
        ),
    )
    parser_cli.add_argument(
        "--custom-data-size",
        type=int,
        required=True,
        help="Exact rebuilt data_size of the custom geometry node.",
    )
    parser_cli.add_argument(
        "--output",
        type=str,
        required=True,
        help="Path for the aligned output manifest.",
    )
    args = parser_cli.parse_args()

    original_hash = int(args.original_hash, 16)
    custom_node_id = int(args.custom_node_id, 16)

    print(f"[*] Loading manifest: {args.manifest}")
    esf = load_manifest(args.manifest)

    print(f"[*] Searching for pointer to original asset 0x{original_hash:08X} ...")
    pointer_node = find_pointer_index(esf, original_hash)
    if pointer_node is None:
        print(f"[-] Pointer not found for hash 0x{original_hash:08X}")
        return 1

    print(f"[*] Overwriting pointer to custom node 0x{custom_node_id:05X} ...")
    align_pointer_to_custom_geometry(
        esf, pointer_node, custom_node_id, args.custom_data_size
    )

    print(f"[*] Rebuilding aligned manifest ...")
    rebuild_and_save(esf, args.output)

    print("[+] Manifest alignment complete.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
