#!/usr/bin/env python3
"""
iso_rip_and_render.py
=====================
End-to-end visual verification of an EQOA Frontiers patched ISO.

Pipeline:
  1. Open the patched ISO (raw, no mount required).
  2. Locate the LBA + size of a target .CSF / .ESF file by walking the
     ISO9660 directory records directly (does not trust pycdlib because the
     surgical LBA patching in repack_iso.py can produce records that strict
     parsers reject).
  3. Read the raw bytes from that LBA range and dump them to a temp folder.
  4. Pipe the extracted file straight into core/visual_vif_renderer.py so a
     3D matplotlib window opens. If the geometry looks right, repacking is
     verified end-to-end -- no booting PCSX2 required.

Usage:
    python iso_rip_and_render.py iso/patched/EQOA_Frontiers_Patched.iso
    python iso_rip_and_render.py <iso> --file CHAR.ESF
    python iso_rip_and_render.py <iso> --file CHARSEL1.CSF --hash 0x05AEBA67
    python iso_rip_and_render.py <iso> --keep        # do not delete temp dump
"""

import argparse
import os
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

SECTOR_SIZE = 2048
PVD_LBA = 16


# ---------------------------------------------------------------------------
# Raw ISO9660 walker
# ---------------------------------------------------------------------------
def _read_sector(fp, lba, count=1):
    fp.seek(lba * SECTOR_SIZE)
    return fp.read(SECTOR_SIZE * count)


def _parse_dir_records(block):
    """Yield (name, lba, size, is_dir) from a directory extent block."""
    pos = 0
    while pos < len(block):
        if pos >= len(block):
            return
        rec_len = block[pos]
        if rec_len == 0:
            # Pad to next logical sector boundary
            next_sector = ((pos // SECTOR_SIZE) + 1) * SECTOR_SIZE
            if next_sector >= len(block):
                return
            pos = next_sector
            continue
        if pos + rec_len > len(block):
            return
        rec = block[pos:pos + rec_len]
        try:
            lba = struct.unpack('<I', rec[2:6])[0]
            size = struct.unpack('<I', rec[10:14])[0]
            flags = rec[25]
            name_len = rec[32]
            name = rec[33:33 + name_len]
            is_dir = bool(flags & 0x02)
            try:
                name_str = name.decode('ascii', errors='replace')
            except Exception:
                name_str = repr(name)
            yield name_str, lba, size, is_dir
        except Exception:
            pass
        pos += rec_len


def _walk_iso(fp, lba, size, path='/', visited=None):
    """Recursively walk the ISO9660 tree, yielding (full_path, lba, size)."""
    if visited is None:
        visited = set()
    if lba in visited:
        return
    visited.add(lba)

    sectors_needed = (size + SECTOR_SIZE - 1) // SECTOR_SIZE
    block = _read_sector(fp, lba, sectors_needed)

    for name, child_lba, child_size, is_dir in _parse_dir_records(block):
        if name in ('\x00', '\x01'):
            # '.' and '..'
            continue
        if not name:
            continue
        full = path.rstrip('/') + '/' + name
        if is_dir:
            yield from _walk_iso(fp, child_lba, child_size, full, visited)
        else:
            yield full, child_lba, child_size


def find_file_in_iso(iso_path, target_name):
    """Return (full_path, lba, size) for the first file whose name contains target_name."""
    target_upper = target_name.upper()
    with open(iso_path, 'rb') as fp:
        pvd = _read_sector(fp, PVD_LBA)
        if pvd[:6] != b'\x01CD001':
            raise RuntimeError(f"PVD signature missing at LBA {PVD_LBA} -- not a valid ISO9660 image.")

        # The root directory record lives at offset 156 in the PVD, length 34.
        root_rec = pvd[156:156 + 34]
        root_lba = struct.unpack('<I', root_rec[2:6])[0]
        root_size = struct.unpack('<I', root_rec[10:14])[0]

        matches = []
        for path, lba, size in _walk_iso(fp, root_lba, root_size):
            base = path.rsplit('/', 1)[-1]
            # Strip ISO9660 ";1" version suffix for comparison
            clean = base.split(';', 1)[0].upper()
            if target_upper in clean:
                matches.append((path, lba, size))

        if not matches:
            return None

        # Prefer an exact filename match (ignoring version)
        for path, lba, size in matches:
            base = path.rsplit('/', 1)[-1].split(';', 1)[0].upper()
            if base == target_upper:
                return path, lba, size
        return matches[0]


def extract_lba_range(iso_path, lba, size, out_path):
    """Dump `size` bytes starting at sector `lba` from `iso_path` into `out_path`."""
    with open(iso_path, 'rb') as fp:
        fp.seek(lba * SECTOR_SIZE)
        remaining = size
        with open(out_path, 'wb') as out:
            while remaining > 0:
                chunk = fp.read(min(remaining, 4 * 1024 * 1024))
                if not chunk:
                    break
                out.write(chunk)
                remaining -= len(chunk)
    return out_path


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(
        description="Rip a .CSF/.ESF from a patched ISO by LBA and render it in 3D."
    )
    ap.add_argument("iso", help="Path to the patched .iso file")
    ap.add_argument(
        "--file",
        default="CHAR.ESF",
        help="Filename (or substring) to locate inside the ISO. "
             "Default: CHAR.ESF. Examples: CHARSEL1.CSF, CHARCUST.CSF",
    )
    ap.add_argument(
        "--hash",
        default=None,
        help="Optional asset hash (hex, e.g. 0x05AEBA67) forwarded to visual_vif_renderer.py",
    )
    ap.add_argument(
        "--out-dir",
        default=None,
        help="Directory to write the extracted asset into. Default: system temp.",
    )
    ap.add_argument(
        "--keep",
        action="store_true",
        help="Keep the extracted file on disk after the renderer closes.",
    )
    ap.add_argument(
        "--no-render",
        action="store_true",
        help="Just rip the file, don't launch the renderer (useful for CI).",
    )
    args = ap.parse_args()

    iso_path = os.path.abspath(args.iso)
    if not os.path.isfile(iso_path):
        print(f"[-] ISO not found: {iso_path}")
        sys.exit(2)

    print(f"[*] Opening ISO        : {iso_path}")
    print(f"[*] Locating asset     : {args.file}")

    try:
        result = find_file_in_iso(iso_path, args.file)
    except Exception as e:
        print(f"[-] Failed to walk ISO9660 tree: {e}")
        sys.exit(3)

    if not result:
        print(f"[-] No file matching '{args.file}' was found inside the ISO.")
        sys.exit(4)

    full_path, lba, size = result
    print(f"[+] Found              : {full_path}")
    print(f"    LBA                : {lba}  (byte offset 0x{lba * SECTOR_SIZE:X})")
    print(f"    Size               : {size:,} bytes")

    # Choose output directory
    if args.out_dir:
        out_dir = Path(args.out_dir)
        out_dir.mkdir(parents=True, exist_ok=True)
        tmp_handle = None
    else:
        tmp_handle = tempfile.mkdtemp(prefix="iso_rip_")
        out_dir = Path(tmp_handle)

    base_name = full_path.rsplit('/', 1)[-1].split(';', 1)[0]
    out_path = out_dir / base_name
    print(f"[*] Extracting raw bytes -> {out_path}")
    extract_lba_range(iso_path, lba, size, out_path)
    extracted_size = os.path.getsize(out_path)
    if extracted_size != size:
        print(f"[!] Warning: extracted {extracted_size:,} bytes vs directory record {size:,}")
    else:
        print(f"[+] Extracted {extracted_size:,} bytes (matches directory record).")

    if args.no_render:
        print(f"[*] --no-render set. Dump retained at: {out_path}")
        return

    # --- Launch the visual VIF renderer ---
    project_root = Path(__file__).resolve().parent
    renderer = project_root / "core" / "offline_vif_viewer.py"
    if not renderer.is_file():
        print(f"[-] Renderer not found at {renderer}")
        sys.exit(5)

    cmd = [sys.executable, str(renderer), str(out_path)]
    if args.hash:
        cmd += ["--hash", args.hash]

    print("[*] Launching visual_vif_renderer.py ...")
    print("    " + " ".join(cmd))
    print("    >> If a 3D matplotlib window opens with a coherent character mesh,")
    print("       the ISO repack is VISUALLY VERIFIED.")

    try:
        rc = subprocess.call(cmd, cwd=str(project_root))
    except KeyboardInterrupt:
        rc = 130

    if not args.keep and tmp_handle is not None:
        try:
            os.remove(out_path)
            os.rmdir(tmp_handle)
            print(f"[*] Cleaned temp dir: {tmp_handle}")
        except OSError:
            pass
    else:
        print(f"[*] Extracted file retained at: {out_path}")

    sys.exit(rc)


if __name__ == "__main__":
    main()
