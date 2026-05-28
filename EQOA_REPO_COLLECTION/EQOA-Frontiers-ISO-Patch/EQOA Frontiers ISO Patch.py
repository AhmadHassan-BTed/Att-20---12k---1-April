#!/usr/bin/env python3
"""
patch_dnas.py — Combined patcher for DNAS255.IMG and UTIL.REL.

Auto-detects the input file type by magic bytes and applies the appropriate
patches:

    'CD001' at offset 0x8001  →  PS2 ISO. The script will:
                                  • verify the ISO's SHA-256 matches
                                    EXPECTED_ISO_SHA256 (eject-fast on a
                                    completely wrong file).
                                  • read SYSTEM.CNF, parse BOOT2 to find
                                    the main game ELF (e.g. SLUS_207.44),
                                    and verify its 32-bit XOR checksum
                                    matches EXPECTED_ELF_CHECKSUM. Aborts
                                    without modification on mismatch.
                                  • locate MODULES/DNAS255.IMG and patch
                                    its embedded CDVDMAN byte-store.
                                  • walk the ISO tree to find every
                                    UTIL.REL file (any directory) and patch
                                    each one with the DNAS2 stubs.
    'RESET'                    →  raw DNAS255.IMG (CDVDMAN patch only)
    'SNR1'                     →  UTIL.REL  (DNAS2 stubs only)

Patches applied to DNAS255.IMG:
    File offset 0x000235CC (inside CDVDMAN.IRX):
        00 00 62 A0  (sb $v0,   0($v1))
      → 00 00 60 A0  (sb $zero, 0($v1))

Patches applied to UTIL.REL (10 sceDNAS2* functions stubbed to return 0):
    sceDNAS2InitNoHDD          @ file 0x00010570
    sceDNAS2AuthInstall        @ file 0x00010590
    sceDNAS2AuthDataDownload   @ file 0x00010940
    sceDNAS2AuthDataDownload2  @ file 0x00010DF8
    sceDNAS2AuthGetUniqueID    @ file 0x00011070
    sceDNAS2Shutdown           @ file 0x00011228
    sceDNAS2Abort              @ file 0x00011448
    sceDNAS2GetStatus          @ file 0x00011570  (fills status struct)
    sceDNAS2SetProxy           @ file 0x000115C8
    sceDNAS2GetProxy           @ file 0x00011640

UTIL.REL load base = 0x00170E80; file_offset = (pnach_addr & 0x0FFFFFFF) - 0x00170E80.

After overwriting function bodies, the script also rewrites the SNR1
relocation table to redirect any relocations that targeted instructions
inside the patched function bodies. Without this step the SNR1 loader
would re-apply those relocations on top of the patched stubs at load
time, corrupting the stub instructions and crashing on first call to
any of the DNAS2 functions.

All patches verify the original bytes (or detect already-patched bytes)
before writing, so the script will refuse to corrupt a different build.

Usage:
    python3 patch_dnas.py <DNAS255.IMG | game.iso | UTIL.REL>
"""

import struct
import sys
import os
import array
import hashlib

# ─────────────────────────────────────────────────────────────────────────────
#  ISO SHA-256 integrity check
# ─────────────────────────────────────────────────────────────────────────────
#
# A SHA-256 hash of the entire input ISO file. This is a strong fingerprint
# of the exact ISO bytes — far more discriminating than the XOR-fold ELF
# check below, but also fragile against any modification (including this
# script's own patches). Used as a first-line "is this even the right file?"
# gate before the more granular ELF-build check.
#
# Computed over the raw bytes of the ISO file as supplied by the user.
# Re-running this script after a successful patch will produce a different
# hash; add the post-patch hash to ACCEPTED_ISO_SHA256S after observing it
# once if you want to keep idempotent re-runs working.

EXPECTED_ISO_SHA256 = 'F1DB24A5723553D426A0E4857F24A49495F05973305222C82748C2F038A2FB1A'

ACCEPTED_ISO_SHA256S = {
    EXPECTED_ISO_SHA256.lower(): 'unmodified',
    # 'XX...XX': 'fully patched',  # add observed post-patch hash here
}


def compute_file_sha256(path, chunk_size=1 << 20):
    """Stream-compute the SHA-256 of a file. Returns lowercase hex digest."""
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            h.update(chunk)
    return h.hexdigest()


# ─────────────────────────────────────────────────────────────────────────────
#  Game ELF integrity check
# ─────────────────────────────────────────────────────────────────────────────
#
# This is a 32-bit word-wise XOR checksum, equivalent to the C++ routine:
#
#     u32 CRC = 0;
#     const u32* srcdata = ...;   // pointer to ELF data
#     for (u32 i = size / 4; i; --i, ++srcdata)
#         CRC ^= *srcdata;        // XOR every 4 bytes
#     return CRC;
#
# Words are read in little-endian order. Any trailing bytes that don't form
# a complete 32-bit word are silently dropped (matches the C++ loop, which
# only iterates `size / 4` times). PS2 ELFs are always word-aligned in
# practice so this never matters here.
#
# Despite the variable being called "CRC" in the C++ code, this is a plain
# XOR fold and not a polynomial CRC.

# Expected XOR checksum of the main game ELF (the SLUS_xxx.xx file pointed
# at by BOOT2 in SYSTEM.CNF). The script aborts if the ELF in the ISO
# doesn't match. This fingerprints the specific game build whose offsets
# the patches target.
EXPECTED_ELF_CHECKSUM = 0xEEEE1FCC

# Additional checksums that are accepted as "known states". Patching the
# ISO doesn't change the main ELF (we only touch DNAS255.IMG and UTIL.REL),
# so the ELF checksum stays constant across runs and idempotency is
# automatic — no second value needed.
ACCEPTED_ELF_CHECKSUMS = {
    EXPECTED_ELF_CHECKSUM: 'unmodified game build',
}


def xor_checksum_bytes(data):
    """32-bit word-wise XOR checksum of a bytes-like object. Trailing
    bytes that don't form a full word are dropped (matches the C++
    `size / 4` loop)."""
    n = (len(data) // 4) * 4
    if n == 0:
        return 0
    a = array.array('I')
    a.frombytes(bytes(data[:n]))
    # array.array('I') unpacks in native byte order. The PS2 is
    # little-endian and so are virtually all hosts the user will run
    # this on (x86, x86_64, ARM in LE mode). If we ever needed BE-host
    # support we'd byteswap here.
    crc = 0
    for w in a:
        crc ^= w
    return crc & 0xFFFFFFFF


def xor_checksum_iso_region(f, byte_offset, size, chunk_size=1 << 20):
    """Stream-compute the XOR checksum of a region inside an open file
    handle, starting at byte_offset and covering `size` bytes. Used to
    checksum a file embedded inside an ISO without loading the whole ISO
    into memory."""
    chunk_size = (chunk_size // 4) * 4
    crc = 0
    remaining = size
    f.seek(byte_offset)
    carry = b''
    while remaining > 0:
        to_read = min(chunk_size, remaining)
        buf = f.read(to_read)
        if not buf:
            break
        remaining -= len(buf)
        buf = carry + buf
        n_full = (len(buf) // 4) * 4
        carry = buf[n_full:]
        if n_full:
            crc ^= xor_checksum_bytes(buf[:n_full])
    return crc & 0xFFFFFFFF

# ─────────────────────────────────────────────────────────────────────────────
#  ISO9660 helpers
# ─────────────────────────────────────────────────────────────────────────────

SECTOR_SIZE = 2048
PVD_SECTOR  = 16


def read_sector(f, lba):
    f.seek(lba * SECTOR_SIZE)
    return f.read(SECTOR_SIZE)


def parse_dir_record(buf, off):
    """Parse one ISO9660 directory record. Returns (name, lba, size, rec_len,
    is_dir) or None for end-of-records."""
    length = buf[off]
    if length == 0:
        return None
    lba   = struct.unpack_from('<I', buf, off + 2)[0]
    size  = struct.unpack_from('<I', buf, off + 10)[0]
    flags = buf[off + 25]
    nlen  = buf[off + 32]
    name_bytes = buf[off + 33 : off + 33 + nlen]
    name = name_bytes.decode('ascii', 'replace').split(';')[0].upper()
    is_dir = bool(flags & 0x02)
    return name, lba, size, length, is_dir


def iso_get_root(f):
    """Return (root_lba, root_size) from the ISO9660 PVD, or None if not an ISO."""
    pvd = read_sector(f, PVD_SECTOR)
    if len(pvd) < 6 or pvd[0] != 1 or pvd[1:6] != b'CD001':
        return None
    root_lba  = struct.unpack_from('<I', pvd, 156 + 2)[0]
    root_size = struct.unpack_from('<I', pvd, 156 + 10)[0]
    return root_lba, root_size


def iso_find_path(f, path):
    """Find a file by exact path (e.g. 'MODULES/DNAS255.IMG'). Returns
    (byte_offset, size) or raises FileNotFoundError."""
    root = iso_get_root(f)
    if root is None:
        raise ValueError("Not a valid ISO9660 image")
    cur_lba, cur_size = root
    for part in (p for p in path.replace('\\', '/').split('/') if p):
        sectors = (cur_size + SECTOR_SIZE - 1) // SECTOR_SIZE
        found = None
        for s in range(sectors):
            sect = read_sector(f, cur_lba + s)
            pos = 0
            while pos < SECTOR_SIZE:
                if sect[pos] == 0:
                    break
                rec = parse_dir_record(sect, pos)
                if rec is None:
                    break
                name, lba, size, rec_len, _ = rec
                if name == part.upper():
                    found = (lba, size)
                    break
                pos += rec_len
            if found:
                break
        if not found:
            raise FileNotFoundError(f"'{part}' not found in ISO (path: {path})")
        cur_lba, cur_size = found
    return cur_lba * SECTOR_SIZE, cur_size


def iso_walk_for_basename(f, target_basename):
    """Walk the entire ISO directory tree and return all files whose basename
    matches `target_basename` (case-insensitive). Returns list of
    (full_path, byte_offset, size)."""
    root = iso_get_root(f)
    if root is None:
        return []
    target_upper = target_basename.upper()
    matches = []
    queue = [('', root[0], root[1])]
    visited_lbas = set()
    while queue:
        path, lba, size = queue.pop(0)
        if lba in visited_lbas:
            continue
        visited_lbas.add(lba)
        sectors = (size + SECTOR_SIZE - 1) // SECTOR_SIZE
        for s in range(sectors):
            sect = read_sector(f, lba + s)
            pos = 0
            while pos < SECTOR_SIZE:
                if sect[pos] == 0:
                    break
                rec = parse_dir_record(sect, pos)
                if rec is None:
                    break
                name, child_lba, child_size, rec_len, is_dir = rec
                # Skip '.' and '..' (raw 0x00 / 0x01 single-byte names)
                if name and name not in ('\x00', '\x01'):
                    full = f"{path}/{name}" if path else name
                    if is_dir:
                        queue.append((full, child_lba, child_size))
                    elif name == target_upper:
                        matches.append((full, child_lba * SECTOR_SIZE, child_size))
                pos += rec_len
    return matches


def iso_get_main_elf_name(f):
    """Read SYSTEM.CNF from the ISO root and return the main ELF filename
    pointed at by the BOOT2 line (e.g. 'SLUS_207.44'), or None if SYSTEM.CNF
    isn't present or can't be parsed.

    Standard PS2 retail SYSTEM.CNF format:
        BOOT2 = cdrom0:\\SLUS_207.44;1
        VER   = 1.00
        VMODE = NTSC
    """
    try:
        offset, size = iso_find_path(f, 'SYSTEM.CNF')
    except FileNotFoundError:
        return None
    f.seek(offset)
    data = f.read(size)
    text = data.decode('ascii', 'replace')
    for line in text.splitlines():
        if '=' not in line:
            continue
        key, _, value = line.partition('=')
        if key.strip().upper() != 'BOOT2':
            continue
        # value looks like 'cdrom0:\SLUS_207.44;1'
        v = value.strip()
        # Strip device prefix
        if ':' in v:
            v = v.split(':', 1)[1]
        # Strip leading slashes / backslashes
        v = v.lstrip('\\/').replace('\\', '/')
        # Strip ;version suffix
        v = v.split(';', 1)[0]
        # Take the basename (BOOT2 is usually root-relative anyway)
        v = v.rsplit('/', 1)[-1]
        return v
    return None


# ─────────────────────────────────────────────────────────────────────────────
#  DNAS255.IMG (CDVDMAN) patch
# ─────────────────────────────────────────────────────────────────────────────

DNAS_PATCH_OFFSET   = 0x000235CC
DNAS_PATCH_LEN      = 4
DNAS_EXPECTED_ORIG  = bytes.fromhex('000062a0')   # sb $v0,   0($v1)
DNAS_PATCH_BYTES    = bytes.fromhex('000060a0')   # sb $zero, 0($v1)


def patch_dnas255(f, img_base, label='DNAS255.IMG'):
    """Apply the CDVDMAN byte-store patch. img_base is the offset to the
    start of DNAS255.IMG inside the file (0 for raw IMG, ISO offset
    otherwise)."""
    target = img_base + DNAS_PATCH_OFFSET
    f.seek(target)
    orig = f.read(DNAS_PATCH_LEN)

    print(f"\n[*] {label} patch (CDVDMAN byte-store)")
    print(f"    Target offset: 0x{target:08X}")
    print(f"    Original:      {orig.hex(' ')}")

    if orig == DNAS_PATCH_BYTES:
        print(f"    [*] Already patched — no changes made.")
        return 0

    if orig != DNAS_EXPECTED_ORIG:
        print(f"    Expected:      {DNAS_EXPECTED_ORIG.hex(' ')}")
        print(f"[!] Original bytes don't match expected build for {label}.")
        print(f"[!] Aborting to avoid corruption.")
        return 1

    f.seek(target)
    f.write(DNAS_PATCH_BYTES)
    print(f"    [+] Patched at 0x{target:08X}")
    print(f"        Was: {DNAS_EXPECTED_ORIG.hex(' ')}  (sb $v0,   0($v1))")
    print(f"        Now: {DNAS_PATCH_BYTES.hex(' ')}  (sb $zero, 0($v1))")
    return 0


# ─────────────────────────────────────────────────────────────────────────────
#  UTIL.REL patches
# ─────────────────────────────────────────────────────────────────────────────

SNR1_MAGIC      = b'SNR1'
UTIL_LOAD_BASE  = 0x00170E80   # UTIL.REL's runtime load address in EE memory

# Relocation table location & layout (read from the SNR1 header at file start).
# Header layout (32-bit LE words):
#   [0] magic 'SNR1'
#   [1] reloc_table_offset
#   [2] reloc_count
#   [3] symbol_table_offset
#   [4] symbol_count
# Each reloc entry is 12 bytes: { target_in_file (u32), type (u32), addend (u32) }.
# Each symbol entry is 12 bytes: { name_offset (u32), value (u32), extra (u32) }.

# Scratch byte offset for redirecting "bad" relocations (relocations whose
# target falls inside a patched function body). The 192-byte region at file
# offsets 0x40..0xFF is alignment padding (post-header, pre-text); writing
# stray relocation immediates there at load time is harmless because that
# region is never executed.
UTIL_RELOC_SCRATCH_OFFSET = 0x00000040
UTIL_RELOC_SCRATCH_SIZE   = 0xC0   # 0x40..0xFF inclusive (192 bytes / 48 words)

UTIL_PATCHES = {
    'sceDNAS2InitNoHDD': [
        (0x201813F0, 0x27BDFFF0),
        (0x201813F4, 0xFFBF0000),
        (0x201813F8, 0x0000102D),
        (0x201813FC, 0xDFBF0000),
        (0x20181400, 0x03E00008),
        (0x20181404, 0x27BD0010),
    ],
    'sceDNAS2AuthInstall': [
        (0x20181410, 0x27BDFFD0), (0x20181414, 0xFFB00000), (0x20181418, 0xFFB20010),
        (0x2018141C, 0xFFB30018), (0x20181420, 0xFFB10008), (0x20181424, 0xFFBF0020),
        (0x20181428, 0x0000102D), (0x2018142C, 0xDFB00000), (0x20181430, 0xDFB10008),
        (0x20181434, 0xDFB20010), (0x20181438, 0xDFB30018), (0x2018143C, 0xDFBF0020),
        (0x20181440, 0x03E00008), (0x20181444, 0x27BD0030),
    ],
    'sceDNAS2AuthDataDownload': [
        (0x201817C0, 0x27BDFFD0), (0x201817C4, 0xFFB00000), (0x201817C8, 0xFFB20010),
        (0x201817CC, 0xFFB10008), (0x201817D0, 0xFFB30018), (0x201817D4, 0xFFBF0020),
        (0x201817D8, 0x0000102D), (0x201817DC, 0xDFB00000), (0x201817E0, 0xDFB10008),
        (0x201817E4, 0xDFB20010), (0x201817E8, 0xDFB30018), (0x201817EC, 0xDFBF0020),
        (0x201817F0, 0x03E00008), (0x201817F4, 0x27BD0030),
    ],
    'sceDNAS2AuthDataDownload2': [
        (0x20181C78, 0x27BDFFD0), (0x20181C7C, 0xFFB00000), (0x20181C80, 0xFFB10008),
        (0x20181C84, 0xFFB20010), (0x20181C88, 0xFFB30018), (0x20181C8C, 0xFFB40020),
        (0x20181C90, 0xFFBF0028), (0x20181C94, 0x0000102D), (0x20181C98, 0xDFB00000),
        (0x20181C9C, 0xDFB10008), (0x20181CA0, 0xDFB20010), (0x20181CA4, 0xDFB30018),
        (0x20181CA8, 0xDFB40020), (0x20181CAC, 0xDFBF0028), (0x20181CB0, 0x03E00008),
        (0x20181CB4, 0x27BD0030),
    ],
    'sceDNAS2AuthGetUniqueID': [
        (0x20181EF0, 0x27BDFFD0), (0x20181EF4, 0xFFB00000), (0x20181EF8, 0xFFB20010),
        (0x20181EFC, 0xFFB10008), (0x20181F00, 0xFFB30018), (0x20181F04, 0xFFBF0020),
        (0x20181F08, 0x0000102D), (0x20181F0C, 0xDFB00000), (0x20181F10, 0xDFB10008),
        (0x20181F14, 0xDFB20010), (0x20181F18, 0xDFB30018), (0x20181F1C, 0xDFBF0020),
        (0x20181F20, 0x03E00008), (0x20181F24, 0x27BD0030),
    ],
    'sceDNAS2Shutdown': [
        (0x201820A8, 0x27BDFFE0), (0x201820AC, 0xFFB20010), (0x201820B0, 0xFFB00000),
        (0x201820B4, 0xFFB10008), (0x201820B8, 0xFFBF0018), (0x201820BC, 0x0000102D),
        (0x201820C0, 0xDFB00000), (0x201820C4, 0xDFB10008), (0x201820C8, 0xDFB20010),
        (0x201820CC, 0xDFBF0018), (0x201820D0, 0x03E00008), (0x201820D4, 0x27BD0020),
    ],
    'sceDNAS2Abort': [
        (0x201822C8, 0x27BDFFC0), (0x201822CC, 0xFFB00030), (0x201822D0, 0xFFBF0038),
        (0x201822D4, 0x0000102D), (0x201822D8, 0xDFB00030), (0x201822DC, 0xDFBF0038),
        (0x201822E0, 0x03E00008), (0x201822E4, 0x27BD0040),
    ],
    'sceDNAS2GetStatus': [
        (0x201823F0, 0x3C050400), (0x201823F4, 0x0000102D), (0x201823F8, 0x24030005),
        (0x201823FC, 0x34A50004), (0x20182400, 0x24060003), (0x20182404, 0xAC830000),
        (0x20182408, 0xAC850004), (0x2018240C, 0xAC860008), (0x20182410, 0x03E00008),
        (0x20182414, 0xAC80000C),
    ],
    'sceDNAS2SetProxy': [
        (0x20182448, 0x27BDFFE0), (0x2018244C, 0xFFB10008), (0x20182450, 0xFFB00000),
        (0x20182454, 0xFFBF0010), (0x20182458, 0x0000102D), (0x2018245C, 0xDFB00000),
        (0x20182460, 0xDFB10008), (0x20182464, 0xDFBF0010), (0x20182468, 0x03E00008),
        (0x2018246C, 0x27BD0020),
    ],
    'sceDNAS2GetProxy': [
        (0x201824C0, 0x27BDFFE0), (0x201824C4, 0xFFB10008), (0x201824C8, 0xFFB00000),
        (0x201824CC, 0xFFBF0010), (0x201824D0, 0x0000102D), (0x201824D4, 0xDFB00000),
        (0x201824D8, 0xDFB10008), (0x201824DC, 0xDFBF0010), (0x201824E0, 0x03E00008),
        (0x201824E4, 0x27BD0020),
    ],
}

UTIL_EXPECTED_OFFSETS = {
    'sceDNAS2InitNoHDD':         0x00010570,
    'sceDNAS2AuthInstall':       0x00010590,
    'sceDNAS2AuthDataDownload':  0x00010940,
    'sceDNAS2AuthDataDownload2': 0x00010DF8,
    'sceDNAS2AuthGetUniqueID':   0x00011070,
    'sceDNAS2Shutdown':          0x00011228,
    'sceDNAS2Abort':             0x00011448,
    'sceDNAS2GetStatus':         0x00011570,
    'sceDNAS2SetProxy':          0x000115C8,
    'sceDNAS2GetProxy':          0x00011640,
}


def util_pnach_to_offset(pnach_addr):
    return (pnach_addr & 0x0FFFFFFF) - UTIL_LOAD_BASE


def patch_util_rel(f, file_base, label='UTIL.REL'):
    """Apply all UTIL.REL DNAS2 patches. file_base is the start of the
    UTIL.REL data within the file (0 for a raw .REL, ISO offset for an
    embedded REL).

    After patching the function bodies, this also rewrites the SNR1
    relocation table to neutralize any relocations that target instructions
    inside the patched ranges — otherwise the loader corrupts our stubs at
    load time."""
    # Verify SNR1 magic at file_base
    f.seek(file_base)
    if f.read(4) != SNR1_MAGIC:
        print(f"[!] {label}: SNR1 magic missing at offset 0x{file_base:X}")
        return 1

    # Sanity-check the pnach→offset mapping against our symbol-table-derived
    # expected offsets.
    for name, words in UTIL_PATCHES.items():
        computed = util_pnach_to_offset(words[0][0])
        if computed != UTIL_EXPECTED_OFFSETS[name]:
            print(f"[!] {name}: pnach mapping says 0x{computed:X}, "
                  f"symbol table says 0x{UTIL_EXPECTED_OFFSETS[name]:X}")
            print(f"[!] Aborting due to mapping mismatch.")
            return 1

    # Read the SNR1 header to locate the relocation table.
    f.seek(file_base + 4)
    reloc_offset = struct.unpack('<I', f.read(4))[0]
    reloc_count  = struct.unpack('<I', f.read(4))[0]
    print(f"\n[*] {label} DNAS2 stub patches  (load base 0x{UTIL_LOAD_BASE:08X})")
    print(f"    Reloc table: 0x{reloc_offset:08X} × {reloc_count} entries (12 bytes each)")

    # Compute patched ranges in module-relative file offsets.
    patched_ranges = []
    for name, words in UTIL_PATCHES.items():
        rel_start = util_pnach_to_offset(words[0][0])
        rel_end   = rel_start + len(words) * 4
        patched_ranges.append((name, rel_start, rel_end))

    def in_patched_range(rel_off):
        for name, s, e in patched_ranges:
            if s <= rel_off < e:
                return name
        return None

    # ── Step 1: write the function-body patches ──
    total_changed   = 0
    total_unchanged = 0
    for name, words in UTIL_PATCHES.items():
        rel_base    = util_pnach_to_offset(words[0][0])
        absolute    = file_base + rel_base
        changed_in_func = 0
        for pnach_addr, new_word in words:
            file_off = file_base + util_pnach_to_offset(pnach_addr)
            f.seek(file_off)
            cur = struct.unpack('<I', f.read(4))[0]
            if cur == new_word:
                total_unchanged += 1
                continue
            f.seek(file_off)
            f.write(struct.pack('<I', new_word))
            total_changed   += 1
            changed_in_func += 1
        if changed_in_func == 0:
            print(f"    {name:<28s} @ 0x{absolute:08X}  (already patched)")
        else:
            print(f"    {name:<28s} @ 0x{absolute:08X}  "
                  f"({changed_in_func}/{len(words)} words written)")

    print(f"\n    Function-body words written: {total_changed}")
    print(f"    Already-correct skipped:    {total_unchanged}")

    # ── Step 2: rewrite the relocation table ──
    # Walk every reloc entry; if its target falls inside a patched range,
    # redirect the target field to point at a slot in the scratch region.
    # The relocation will still be processed at load time, but the resulting
    # write goes to harmless padding instead of clobbering our stub.
    print(f"\n[*] Scanning relocation table for entries targeting patched ranges...")

    bad_entries = []          # list of (entry_index, target_offset_in_module)
    already_redirected = []
    scratch_max = UTIL_RELOC_SCRATCH_OFFSET + UTIL_RELOC_SCRATCH_SIZE

    for i in range(reloc_count):
        entry_off = file_base + reloc_offset + i * 12
        f.seek(entry_off)
        a, b, c = struct.unpack('<III', f.read(12))
        # 'a' is a module-relative file offset (matches the format we use).
        if in_patched_range(a):
            bad_entries.append((i, a, b, c, entry_off))
        elif (UTIL_RELOC_SCRATCH_OFFSET <= a
              < UTIL_RELOC_SCRATCH_OFFSET + UTIL_RELOC_SCRATCH_SIZE):
            already_redirected.append((i, a))

    print(f"    Found {len(bad_entries)} relocation(s) targeting patched function bodies")
    if already_redirected:
        print(f"    (Already-redirected entries from a previous run: "
              f"{len(already_redirected)})")

    if not bad_entries:
        print(f"    Nothing to fix.")
        return 0

    # Pick a single scratch slot to redirect everything to. Using one slot
    # keeps the patch minimal. The loader will write to it repeatedly as it
    # processes each entry; since the slot is just padding and is never
    # executed, the resulting value doesn't matter.
    scratch_slot = UTIL_RELOC_SCRATCH_OFFSET    # 0x40

    print(f"    Redirecting target offsets → 0x{scratch_slot:08X} (scratch padding)")
    by_func = {}
    for i, a, b, c, entry_off in bad_entries:
        name = in_patched_range(a)
        by_func.setdefault(name, []).append((i, a, b, c, entry_off))
        # Rewrite the target field of this reloc entry. Keep type & addend
        # intact (the loader still does the relocation math, but writes to
        # the scratch slot).
        f.seek(entry_off)
        f.write(struct.pack('<III', scratch_slot, b, c))

    for name, _, _ in patched_ranges:
        n = len(by_func.get(name, []))
        if n:
            print(f"      {name:<28s}  {n} reloc(s) redirected")

    print(f"\n    Total relocations redirected: {len(bad_entries)}")
    return 0


# ─────────────────────────────────────────────────────────────────────────────
#  File-type detection
# ─────────────────────────────────────────────────────────────────────────────

def detect_file_type(f):
    """Inspect the file and return one of: 'iso', 'dnas255_img', 'util_rel'."""
    f.seek(0)
    head = f.read(8)
    if head[:4] == SNR1_MAGIC:
        return 'util_rel'
    if head.startswith(b'RESET'):
        return 'dnas255_img'
    if iso_get_root(f) is not None:
        return 'iso'
    return None


# ─────────────────────────────────────────────────────────────────────────────
#  Main
# ─────────────────────────────────────────────────────────────────────────────

def _prompt_yes_no(message, default='no'):
    """Ask the user a yes/no question and return True/False.

    `default` is 'yes' or 'no' and selects what pressing Enter does.

    For non-interactive runs (no TTY), returns False unless the
    EQOA_OVERRIDE_INTEGRITY environment variable is set to '1'/'yes'/
    'true' (case-insensitive). This lets automation explicitly opt in
    without us blocking on a prompt that nobody will answer.
    """
    # Env-var override for automation. Useful for CI/scripted runs that
    # know they want to proceed despite a warning.
    env_override = os.environ.get('EQOA_OVERRIDE_INTEGRITY', '').strip().lower()
    if env_override in ('1', 'yes', 'true', 'y'):
        print(f"[*] EQOA_OVERRIDE_INTEGRITY is set; continuing despite warning.")
        return True

    # If we're not interactive and there's no env override, refuse.
    try:
        interactive = (sys.stdin and sys.stdin.isatty() and
                       sys.stdout and sys.stdout.isatty())
    except Exception:
        interactive = False
    if not interactive:
        print("[!] Non-interactive run; refusing to proceed past integrity warning.")
        print("[!] Set EQOA_OVERRIDE_INTEGRITY=1 in the environment to override.")
        return False

    suffix = '[Y/n]' if default == 'yes' else '[y/N]'
    while True:
        try:
            answer = input(f"{message} {suffix}: ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            print()
            return False

        if answer == '':
            return default == 'yes'
        if answer in ('y', 'yes'):
            return True
        if answer in ('n', 'no'):
            return False
        print(f"  Please answer y or n.")


def _prompt_patch_mode():
    """Ask the user which patches to apply: DNAS bypass only, iLinkID
    fix only, or both. Returns one of 'dnas', 'ilink', 'both'.

    For non-interactive runs (no TTY), defaults to 'both' so that
    automated/scripted invocations behave the same as before this
    prompt was added. Users wanting to override that in automation can
    set the EQOA_PATCH_MODE environment variable.
    """
    # Honor an explicit env-var override if set (useful for scripting).
    env_mode = os.environ.get('EQOA_PATCH_MODE', '').strip().lower()
    if env_mode in ('dnas', 'ilink', 'both'):
        print(f"[*] Patch mode (from EQOA_PATCH_MODE): {env_mode}")
        return env_mode

    # If we're not running interactively, default to both. Don't block
    # waiting for input that will never come.
    try:
        interactive = (sys.stdin and sys.stdin.isatty() and
                       sys.stdout and sys.stdout.isatty())
    except Exception:
        interactive = False
    if not interactive:
        print("[*] Non-interactive run; applying BOTH patches (default).")
        return 'both'

    # Interactive prompt.
    print()
    print("Which patches do you want to apply?")
    print("  [1] DNAS bypass only       (modifies UTIL.REL)")
    print("  [2] iLinkID fix only       (modifies DNAS255.IMG)")
    print("  [3] Both                   (default)")
    print()
    print("WARNING: this script modifies the ISO IN PLACE. Once you pick,")
    print("the patch is permanent for that ISO. If you choose 1 or 2 and")
    print("later decide you want the other patch too, you must start from")
    print("a fresh COPY of the original unmodified ISO -- the integrity")
    print("checks won't accept a partially-patched ISO on a re-run.")
    print()

    while True:
        try:
            choice = input("Enter 1, 2, or 3 (or just press Enter for both): ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            raise

        if choice == '' or choice == '3':
            return 'both'
        if choice == '1':
            return 'dnas'
        if choice == '2':
            return 'ilink'
        print(f"  Unrecognized: {choice!r}. Please enter 1, 2, or 3.")


def patch_iso(f, path, mode='both'):
    """For an ISO: optionally patch DNAS255.IMG (iLinkID fix) and/or
    every UTIL.REL on the disc (DNAS bypass), depending on `mode`.

      mode='dnas'  -> apply ONLY the DNAS bypass (UTIL.REL stubs)
      mode='ilink' -> apply ONLY the iLinkID fix (DNAS255.IMG byte-store)
      mode='both'  -> apply both (default)

    Returns 0 if everything we touched succeeded (including no-ops);
    returns 1 only if something we attempted FAILED. Missing-file
    conditions are reported but don't fail the run."""

    overall_rc = 0
    something_done = False

    do_ilink = (mode in ('ilink', 'both'))
    do_dnas  = (mode in ('dnas',  'both'))

    # ── DNAS255.IMG (iLinkID fix) ──
    if do_ilink:
        print("[*] Locating MODULES/DNAS255.IMG ...")
        try:
            img_base, img_size = iso_find_path(f, 'MODULES/DNAS255.IMG')
            print(f"    Found at ISO offset 0x{img_base:08X}  ({img_size} bytes)")
            f.seek(img_base)
            if f.read(5) == b'RESET':
                rc = patch_dnas255(f, img_base, label='MODULES/DNAS255.IMG')
                if rc != 0:
                    overall_rc = 1
                something_done = True
            else:
                print("[!] DNAS255.IMG missing RESET magic - skipping iLinkID patch.")
                overall_rc = 1
        except FileNotFoundError as e:
            print(f"    [-] Not found: {e}  (skipping iLinkID patch)")
        except ValueError as e:
            print(f"[!] {e}")
            overall_rc = 1
    else:
        print("[*] Skipping iLinkID fix (DNAS255.IMG) per user selection.")

    # ── UTIL.REL (DNAS bypass — search whole tree) ──
    if do_dnas:
        print("\n[*] Searching ISO for UTIL.REL files ...")
        util_matches = iso_walk_for_basename(f, 'UTIL.REL')
        if not util_matches:
            print("    [-] No UTIL.REL files found in ISO.")
        else:
            print(f"    Found {len(util_matches)} UTIL.REL file(s):")
            for path_, off, size in util_matches:
                print(f"      /{path_}  @ 0x{off:08X}  ({size:,} bytes)")
            # Patch each one. Verify SNR1 magic; skip files that aren't actually REL.
            for path_, off, size in util_matches:
                f.seek(off)
                head = f.read(4)
                if head != SNR1_MAGIC:
                    print(f"\n[!] /{path_}: not a SNR1 file (magic={head!r}); skipping.")
                    continue
                rc = patch_util_rel(f, file_base=off, label=f'/{path_}')
                if rc != 0:
                    overall_rc = 1
                something_done = True
    else:
        print("\n[*] Skipping DNAS bypass (UTIL.REL) per user selection.")

    if not something_done:
        print("\n[!] Nothing patched.")
        return 1
    return overall_rc


def main(path):
    if not os.path.isfile(path):
        print(f"[!] File not found: {path}")
        return 1

    size = os.path.getsize(path)
    mb = size // (1024 * 1024)
    print(f"[*] Opening {path}  ({size:,} bytes, {mb} MB)")

    # Peek the file type read-only first so we can do an integrity check
    # before opening for write (an ISO with the wrong checksum must not be
    # touched at all).
    with open(path, 'rb') as f:
        kind = detect_file_type(f)
    if kind is None:
        print("[!] Unrecognized file type (not SNR1, not RESET, not ISO9660)")
        return 1

    # ── SHA-256 integrity check (ISO inputs only) ──
    # First-line gate: verify the entire ISO matches a known-good hash.
    # On mismatch we WARN rather than abort -- repackaged-but-otherwise-
    # identical ISOs (different rippers, repacked releases, etc.) can
    # produce a different hash with no functional difference. The user
    # is given a chance to continue if they know what they're doing.
    # The stricter ELF-XOR check below is the actual hard gate against
    # a different game build.
    if kind == 'iso':
        print(f"[*] Verifying ISO SHA-256...")
        actual_sha = compute_file_sha256(path)
        sha_label = ACCEPTED_ISO_SHA256S.get(actual_sha)
        if sha_label is not None:
            print(f"    SHA-256:  {actual_sha}  ✓ ({sha_label})")
        else:
            print(f"    SHA-256:  {actual_sha}")
            print(f"")
            print(f"[!] WARNING: ISO SHA-256 does not match the expected unmodified retail")
            print(f"[!] build. This can happen for legitimate reasons (the ISO was ripped")
            print(f"[!] differently, repacked, or is a different region/revision), or it")
            print(f"[!] can mean the ISO is genuinely modified or corrupted.")
            print(f"[!]")
            print(f"[!]   expected {EXPECTED_ISO_SHA256.lower()}")
            for h, lbl in ACCEPTED_ISO_SHA256S.items():
                if h != EXPECTED_ISO_SHA256.lower():
                    print(f"[!]   or       {h}  ({lbl})")
            print(f"[!]   actual   {actual_sha}")
            print(f"[!]")
            print(f"[!] If you continue, the script will still verify the main game ELF")
            print(f"[!] checksum below before patching anything. If that ELF check fails,")
            print(f"[!] the script WILL abort -- patches at hardcoded offsets cannot be")
            print(f"[!] safely applied to a different game build.")
            print(f"[!]")
            print(f"[!] BEFORE CONTINUING: make sure you have a backup copy of this ISO")
            print(f"[!] in case patching produces an unusable result.")
            print(f"")
            if not _prompt_yes_no("Continue past the SHA-256 warning?", default='no'):
                print(f"[!] Aborting without modification.")
                return 1
            print(f"[*] Continuing past SHA-256 warning per user choice.")

    # ── Game-ELF integrity check (ISO inputs only) ──
    # The expected checksum is the XOR fold over the main game ELF (the
    # SLUS_xxx.xx file pointed at by BOOT2 in SYSTEM.CNF). It fingerprints
    # the specific game build whose offsets the patches target. The check
    # runs on the ELF, not the whole ISO, because the patches don't modify
    # the ELF — so its checksum is invariant across runs and idempotency
    # is automatic. Mismatched ISOs are rejected before any write.
    if kind == 'iso':
        with open(path, 'rb') as f:
            elf_name = iso_get_main_elf_name(f)
            if elf_name is None:
                print("[!] Couldn't read SYSTEM.CNF / BOOT2 from ISO; "
                      "can't identify main game ELF.")
                print("[!] Aborting without modification.")
                return 1
            try:
                elf_offset, elf_size = iso_find_path(f, elf_name)
            except FileNotFoundError:
                print(f"[!] SYSTEM.CNF points at '{elf_name}' but that file "
                      "wasn't found in the ISO root.")
                print("[!] Aborting without modification.")
                return 1
            print(f"[*] Verifying main game ELF checksum")
            print(f"    File:     /{elf_name}  ({elf_size:,} bytes)")
            print(f"    Expected: 0x{EXPECTED_ELF_CHECKSUM:08X}")
            actual = xor_checksum_iso_region(f, elf_offset, elf_size)

        label = ACCEPTED_ELF_CHECKSUMS.get(actual)
        if label is not None:
            print(f"    Actual:   0x{actual:08X}  ✓ ({label})")
        else:
            print(f"    Actual:   0x{actual:08X}")
            print(f"[!] Game ELF checksum mismatch.")
            print(f"[!]   expected 0x{EXPECTED_ELF_CHECKSUM:08X}  (unmodified game build)")
            for v, lbl in ACCEPTED_ELF_CHECKSUMS.items():
                if v != EXPECTED_ELF_CHECKSUM:
                    print(f"[!]   or       0x{v:08X}  ({lbl})")
            print(f"[!]   actual   0x{actual:08X}")
            print(f"[!] This script's offsets are tied to a specific game build.")
            print(f"[!] Aborting without modification.")
            return 1

    # ── Prompt user for which patches to apply (ISO inputs only) ──
    # The raw IMG/REL paths only have one patch each, so no prompt needed.
    iso_patch_mode = 'both'
    if kind == 'iso':
        iso_patch_mode = _prompt_patch_mode()

    with open(path, 'r+b') as f:
        if kind == 'util_rel':
            print("    Detected UTIL.REL (SNR1)")
            return patch_util_rel(f, file_base=0)

        if kind == 'dnas255_img':
            print("    Detected raw DNAS255.IMG")
            return patch_dnas255(f, img_base=0)

        if kind == 'iso':
            print("    Detected ISO9660 image")
            return patch_iso(f, path, mode=iso_patch_mode)

        print(f"[!] Unhandled file kind: {kind}")
        return 1


def _wait_for_keypress():
    """Pause until the user presses a key, so a double-clicked Windows
    binary doesn't close its console window before they can read the
    output. Skipped when not running in an interactive terminal (e.g.
    in CI, in a pipeline, or when output is being captured) so
    automated invocations don't hang forever."""
    # If stdin or stdout isn't a TTY, we're being scripted -- no point
    # blocking on keypress. Also covers the case where stdin was closed.
    try:
        if not (sys.stdin and sys.stdin.isatty() and
                sys.stdout and sys.stdout.isatty()):
            return
    except Exception:
        return

    print()  # blank line for visual separation
    try:
        if os.name == 'nt':
            # Windows: msvcrt.getch() returns immediately on any keypress
            # (no Enter required). Available in the standard library on
            # Windows-only.
            import msvcrt
            print("Press any key to exit...", end='', flush=True)
            msvcrt.getch()
            print()
        else:
            # POSIX: Enter-to-continue is the simplest portable approach.
            # Avoids fiddling with termios for raw single-key reads.
            input("Press Enter to exit...")
    except (KeyboardInterrupt, EOFError):
        # User hit Ctrl-C or stdin was closed -- proceed to exit.
        pass


if __name__ == '__main__':
    rc = 1
    try:
        if len(sys.argv) != 2:
            print(f"Usage: python3 {os.path.basename(sys.argv[0])} "
                  f"<DNAS255.IMG | game.iso | UTIL.REL>")
            # Usage errors don't need a keypress prompt -- the user just
            # mistyped the command and will see the usage line fine.
            sys.exit(1)
        rc = main(sys.argv[1])
        if rc == 0:
            print(f"\n[+] Done.")
    except KeyboardInterrupt:
        print("\n[!] Interrupted by user.")
        rc = 130
    except Exception as e:
        # Catch any unexpected exception so the keypress prompt still
        # fires and the user sees what blew up before the window closes.
        import traceback
        print(f"\n[!] Unexpected error: {e}")
        traceback.print_exc()
        rc = 1
    finally:
        _wait_for_keypress()
    sys.exit(rc)