#!/usr/bin/env python3
"""
live_ram_tracer.py
==================
Dynamic Runtime Memory Analysis Tool for PCSX2 / EQOA Frontiers.

Attaches to the live pcsx2.exe (or pcsx2-qt.exe) process, locates the
PS2 EE Main RAM (32MB contiguous PAGE_READWRITE region), and performs
live entity scanning for injected character model asset hashes.

When a hash is found in live RAM, the tool dumps 1KB before and 2KB after
the match address in clean hex/ASCII format to live_memory_dump.txt.

This lets us see exactly how the Frontiers engine mutated our pointers,
CLUT tables, and asset struct offsets post-load at runtime.

Author: Lead PS2 Reverse Engineer & Exploitation Dev
"""

import sys
import os
import struct
import ctypes
import ctypes.wintypes
import time
import argparse
from datetime import datetime

# ─────────────────────────────────────────────────────────────────────────────
# Diagnostic Logging Accumulator
# ─────────────────────────────────────────────────────────────────────────────

DIAGNOSTIC_LOG = []

def log_diag(msg=""):
    print(msg)
    DIAGNOSTIC_LOG.append(msg)

# ─────────────────────────────────────────────────────────────────────────────
# Win32 Constants & Structures
# ─────────────────────────────────────────────────────────────────────────────

PROCESS_ALL_ACCESS = 0x001F0FFF
PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400
MEM_COMMIT = 0x1000
PAGE_READWRITE = 0x04
PAGE_EXECUTE_READWRITE = 0x40

TH32CS_SNAPPROCESS = 0x00000002
TH32CS_SNAPMODULE = 0x00000008
TH32CS_SNAPMODULE32 = 0x00000010

kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)

class PROCESSENTRY32(ctypes.Structure):
    _fields_ = [
        ("dwSize", ctypes.wintypes.DWORD),
        ("cntUsage", ctypes.wintypes.DWORD),
        ("th32ProcessID", ctypes.wintypes.DWORD),
        ("th32DefaultHeapID", ctypes.POINTER(ctypes.c_ulong)),
        ("th32ModuleID", ctypes.wintypes.DWORD),
        ("cntThreads", ctypes.wintypes.DWORD),
        ("th32ParentProcessID", ctypes.wintypes.DWORD),
        ("pcPriClassBase", ctypes.c_long),
        ("dwFlags", ctypes.wintypes.DWORD),
        ("szExeFile", ctypes.c_char * 260),
    ]

class MODULEENTRY32(ctypes.Structure):
    _fields_ = [
        ("dwSize", ctypes.wintypes.DWORD),
        ("th32ModuleID", ctypes.wintypes.DWORD),
        ("th32ProcessID", ctypes.wintypes.DWORD),
        ("GlblcntUsage", ctypes.wintypes.DWORD),
        ("ProccntUsage", ctypes.wintypes.DWORD),
        ("modBaseAddr", ctypes.c_void_p),
        ("modBaseSize", ctypes.wintypes.DWORD),
        ("hModule", ctypes.wintypes.HMODULE),
        ("szModule", ctypes.c_char * 256),
        ("szExePath", ctypes.c_char * 260),
    ]

class MEMORY_BASIC_INFORMATION(ctypes.Structure):
    _fields_ = [
        ("BaseAddress", ctypes.c_void_p),
        ("AllocationBase", ctypes.c_void_p),
        ("AllocationProtect", ctypes.wintypes.DWORD),
        ("RegionSize", ctypes.c_size_t),
        ("State", ctypes.wintypes.DWORD),
        ("Protect", ctypes.wintypes.DWORD),
        ("Type", ctypes.wintypes.DWORD),
    ]

# ─────────────────────────────────────────────────────────────────────────────
# Target Asset Hashes (the 11 injected Vanilla character models)
# ─────────────────────────────────────────────────────────────────────────────

TARGET_HASHES = [
    0x2EF8E480,
    0x05AEBA67,
    0xB54E4D8A,
    0xCD51EF83,
    0x7C0C8A10,
    0x90BCCCF2,
    0x6074557C,
    0x5BDEA541,
    0xEBB9FC93,
    0x0017A0BD,
    0xB5C785F2,
]

# PS2 EE Main RAM size: 32 MB
PS2_RAM_SIZE = 0x02000000  # 33,554,432 bytes

# Dump window: 1KB before, 2KB after
DUMP_BEFORE = 1024
DUMP_AFTER  = 2048

# ─────────────────────────────────────────────────────────────────────────────
# Helper Memory Reading & Module Resolution Functions
# ─────────────────────────────────────────────────────────────────────────────

def get_module_base(pid, exe_name):
    """Scan process modules to find the base address of the main executable."""
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)
    if snapshot == -1 or snapshot == ctypes.c_void_p(-1).value:
        log_diag(f"  [-] Failed to create modules snapshot: error {ctypes.get_last_error()}")
        return None
    
    me = MODULEENTRY32()
    me.dwSize = ctypes.sizeof(MODULEENTRY32)
    base_addr = None
    
    if kernel32.Module32First(snapshot, ctypes.byref(me)):
        while True:
            mod_name = me.szModule.lower()
            if exe_name.lower().encode() in mod_name:
                base_addr = me.modBaseAddr
                break
            if not kernel32.Module32Next(snapshot, ctypes.byref(me)):
                break
                
    kernel32.CloseHandle(snapshot)
    return base_addr

def read_remote_mem(proc_handle, address, size):
    """Read a small block of memory from the target process."""
    buf = ctypes.create_string_buffer(size)
    bytes_read = ctypes.c_size_t(0)
    ok = kernel32.ReadProcessMemory(
        proc_handle,
        ctypes.c_void_p(address),
        buf,
        size,
        ctypes.byref(bytes_read)
    )
    if not ok:
        return None
    return buf.raw[:bytes_read.value]

def find_eemem_via_export(proc_handle, pid, exe_name):
    """Parse PE headers of PCSX2 process externally to locate the exported 'EEmem' pointer."""
    log_diag(f"\n[*] Attempting to locate 'EEmem' via PE exports of {exe_name}...")
    base_addr = get_module_base(pid, exe_name)
    if not base_addr:
        log_diag("  [-] Could not locate module base address.")
        return None
        
    log_diag(f"  [+] Module base address: 0x{base_addr:X}")
    
    # Read DOS header
    dos_hdr = read_remote_mem(proc_handle, base_addr, 64)
    if not dos_hdr or dos_hdr[:2] != b'MZ':
        log_diag("  [-] Invalid DOS header in remote module.")
        return None
        
    pe_offset = struct.unpack_from('<I', dos_hdr, 0x3c)[0]
    # Read NT headers (up to Optional Header data directories)
    pe_hdr = read_remote_mem(proc_handle, base_addr + pe_offset, 264)
    if not pe_hdr or pe_hdr[:4] != b'PE\x00\x00':
        log_diag("  [-] Invalid PE header signature in remote module.")
        return None
        
    # Check Magic to see if PE32+ (64-bit)
    magic = struct.unpack_from('<H', pe_hdr, 24)[0]
    is_64 = (magic == 0x20B)
    log_diag(f"  [+] Target process architecture: {'64-bit' if is_64 else '32-bit'}")
    
    if is_64:
        export_dir_offset = 24 + 112  # PE32+ Export table directory offset
    else:
        export_dir_offset = 24 + 96   # PE32
        
    export_rva, export_size = struct.unpack_from('<II', pe_hdr, export_dir_offset)
    if export_rva == 0:
        log_diag("  [-] No export table found in remote module.")
        return None
        
    log_diag(f"  [+] Export Directory RVA: 0x{export_rva:X}, Size: {export_size} bytes")
    
    # Read Export Directory Structure (IMAGE_EXPORT_DIRECTORY)
    export_dir_bytes = read_remote_mem(proc_handle, base_addr + export_rva, 40)
    if not export_dir_bytes:
        log_diag("  [-] Failed to read IMAGE_EXPORT_DIRECTORY structure.")
        return None
        
    num_funcs = struct.unpack_from('<I', export_dir_bytes, 20)[0]
    num_names = struct.unpack_from('<I', export_dir_bytes, 24)[0]
    funcs_rva = struct.unpack_from('<I', export_dir_bytes, 28)[0]
    names_rva = struct.unpack_from('<I', export_dir_bytes, 32)[0]
    ords_rva = struct.unpack_from('<I', export_dir_bytes, 36)[0]
    
    log_diag(f"  [+] Total exports: {num_funcs} functions, {num_names} named")
    
    # Read tables
    names_data = read_remote_mem(proc_handle, base_addr + names_rva, num_names * 4)
    ords_data = read_remote_mem(proc_handle, base_addr + ords_rva, num_names * 2)
    funcs_data = read_remote_mem(proc_handle, base_addr + funcs_rva, num_funcs * 4)
    
    if not names_data or not ords_data or not funcs_data:
        log_diag("  [-] Failed to read export table arrays from remote memory.")
        return None
        
    # Search for "EEmem"
    eemem_ptr = None
    for i in range(num_names):
        name_rva = struct.unpack_from('<I', names_data, i * 4)[0]
        # Read name (max 64 bytes)
        name_bytes = read_remote_mem(proc_handle, base_addr + name_rva, 64)
        if not name_bytes:
            continue
        name_str = name_bytes.split(b'\x00')[0].decode('utf-8', errors='replace')
        if name_str == "EEmem":
            ord_val = struct.unpack_from('<H', ords_data, i * 2)[0]
            func_rva = struct.unpack_from('<I', funcs_data, ord_val * 4)[0]
            eemem_ptr = base_addr + func_rva
            log_diag(f"  [+] Found 'EEmem' export at RVA 0x{func_rva:X} (Address: 0x{eemem_ptr:X})")
            break
            
    if not eemem_ptr:
        log_diag("  [-] 'EEmem' export symbol not found in module exports.")
        return None
        
    # Read the 8-byte pointer value stored at eemem_ptr
    ptr_bytes = read_remote_mem(proc_handle, eemem_ptr, 8)
    if not ptr_bytes or len(ptr_bytes) < 8:
        log_diag("  [-] Failed to read 8 bytes at EEmem pointer address.")
        return None
        
    eemem_base = struct.unpack('<Q', ptr_bytes)[0]
    log_diag(f"  [+] SUCCESS! EEmem dereferenced to base: 0x{eemem_base:016X}")
    return eemem_base

# ─────────────────────────────────────────────────────────────────────────────
# Process Discovery
# ─────────────────────────────────────────────────────────────────────────────

def find_pcsx2_pid():
    """Scan running processes to find pcsx2.exe or pcsx2-qt.exe."""
    target_names = [b"pcsx2.exe", b"pcsx2-qt.exe", b"pcsx2x64.exe",
                    b"pcsx2-qtx64.exe", b"pcsx2-avx2.exe"]

    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    if snapshot == ctypes.c_void_p(-1).value or snapshot == -1:
        print("[-] Failed to create process snapshot.")
        return None

    pe = PROCESSENTRY32()
    pe.dwSize = ctypes.sizeof(PROCESSENTRY32)

    found_pid = None
    found_name = None

    if kernel32.Process32First(snapshot, ctypes.byref(pe)):
        while True:
            exe_name = pe.szExeFile.lower()
            for target in target_names:
                if target in exe_name:
                    found_pid = pe.th32ProcessID
                    found_name = pe.szExeFile.decode('utf-8', errors='replace')
                    break
            if found_pid:
                break
            if not kernel32.Process32Next(snapshot, ctypes.byref(pe)):
                break

    kernel32.CloseHandle(snapshot)

    if found_pid:
        print(f"[+] Found PCSX2 process: {found_name} (PID: {found_pid})")
    else:
        print("[-] Could not find pcsx2.exe or pcsx2-qt.exe in running processes.")
        print("    Make sure PCSX2 is running with the game loaded.")

    return found_pid, found_name


def open_process(pid):
    """Open the target process, trying escalating access levels."""
    # Try multiple access masks from most to least privileged
    access_levels = [
        (PROCESS_ALL_ACCESS, "PROCESS_ALL_ACCESS"),
        (PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, "VM_READ|QUERY_INFO"),
        (PROCESS_VM_READ, "VM_READ only"),
    ]
    for access, label in access_levels:
        handle = kernel32.OpenProcess(access, False, pid)
        if handle:
            print(f"[+] Process handle acquired (access={label}): 0x{handle:X}")
            return handle
        err = ctypes.get_last_error()
        print(f"    Tried {label}: failed (error {err})")

    print(f"[-] OpenProcess failed with all access levels.")
    print("    Try running this script as Administrator.")
    return None

# ─────────────────────────────────────────────────────────────────────────────
# EEmem Discovery — Find the 32MB PS2 Main RAM block
# ─────────────────────────────────────────────────────────────────────────────

def find_eemem(proc_handle, pid=None, exe_name=None):
    """
    Find the 32MB contiguous Emotion Engine RAM base address.
    First tries dynamically querying the remote process's exported 'EEmem' symbol (bulletproof on modern PCSX2).
    If that fails, falls back to walking the committed virtual memory pages.
    """
    if pid and exe_name:
        eemem_base = find_eemem_via_export(proc_handle, pid, exe_name)
        if eemem_base:
            log_diag(f"\n[+] EEmem successfully located via exported symbol: 0x{eemem_base:016X}")
            return eemem_base, PS2_RAM_SIZE

    log_diag("\n[*] Falling back to virtual memory page scanning...")
    mbi = MEMORY_BASIC_INFORMATION()
    mbi_size = ctypes.sizeof(mbi)
    
    address = 0
    candidates = []
    large_candidates = []
    
    while address < 0x7FFFFFFFFFFF:  # User-mode address space limit (64-bit)
        result = kernel32.VirtualQueryEx(
            proc_handle,
            ctypes.c_void_p(address),
            ctypes.byref(mbi),
            mbi_size
        )
        if result == 0:
            break

        region_size = mbi.RegionSize
        base = mbi.BaseAddress if mbi.BaseAddress else 0

        if (mbi.State == MEM_COMMIT and
            mbi.Protect in (PAGE_READWRITE, PAGE_EXECUTE_READWRITE)):

            if region_size == PS2_RAM_SIZE:
                candidates.append(base)
            elif region_size >= PS2_RAM_SIZE:
                large_candidates.append((base, region_size))

        address = base + region_size
        if address <= base:  # Overflow guard
            break

    log_diag(f"    Exact 32MB regions found: {len(candidates)}")
    log_diag(f"    Regions >= 32MB found:    {len(large_candidates)}")

    # Validate candidates by reading the first bytes and checking for PS2 signatures
    validated = []
    all_candidates = [(addr, PS2_RAM_SIZE) for addr in candidates] + large_candidates
    
    for base_addr, size in all_candidates:
        buf = (ctypes.c_char * 256)()
        bytes_read = ctypes.c_size_t(0)
        ok = kernel32.ReadProcessMemory(
            proc_handle,
            ctypes.c_void_p(base_addr),
            buf,
            256,
            ctypes.byref(bytes_read)
        )
        if not ok or bytes_read.value < 256:
            continue

        raw = bytes(buf)
        first_word = struct.unpack_from('<I', raw, 0)[0]
        
        # MIPS jump instructions opcodes checks
        is_mips = (first_word >> 26) in (0x02, 0x03, 0x0F, 0x08, 0x09, 0x0D, 0x3C >> 2)
        
        buf_deep = (ctypes.c_char * 4096)()
        ok2 = kernel32.ReadProcessMemory(
            proc_handle,
            ctypes.c_void_p(base_addr),
            buf_deep,
            4096,
            ctypes.byref(bytes_read)
        )
        raw_deep = bytes(buf_deep) if ok2 else b''
        has_slus = b'SLUS' in raw_deep or b'SCUS' in raw_deep
        
        # Check if FJBO magic exists anywhere in first 32MB
        # (too expensive to scan all, check strategic offsets)
        score = 0
        if is_mips:
            score += 2
        if has_slus:
            score += 5
        if size == PS2_RAM_SIZE:
            score += 3
            
        validated.append((base_addr, size, score, first_word))
        log_diag(f"    Candidate: base=0x{base_addr:016X}, size={size:,}, "
                 f"score={score}, first_word=0x{first_word:08X}")

    if not validated:
        log_diag("[-] ERROR: Could not locate EEmem! No valid 32MB RAM regions found.")
        log_diag("    Make sure the game is fully loaded (past title screen) before running.")
        return None, 0

    # Pick highest scoring candidate
    validated.sort(key=lambda x: x[2], reverse=True)
    best = validated[0]
    eemem_base = best[0]
    eemem_size = min(best[1], PS2_RAM_SIZE)

    log_diag(f"\n[+] EEmem located at host address via VM query: 0x{eemem_base:016X}")
    log_diag(f"    Region size: {eemem_size:,} bytes")
    return eemem_base, eemem_size

# ─────────────────────────────────────────────────────────────────────────────
# Live RAM Reading
# ─────────────────────────────────────────────────────────────────────────────

def read_eemem(proc_handle, eemem_base, offset, size):
    """Read `size` bytes from EEmem at the given PS2 offset."""
    host_addr = eemem_base + offset
    buf = (ctypes.c_char * size)()
    bytes_read = ctypes.c_size_t(0)
    ok = kernel32.ReadProcessMemory(
        proc_handle,
        ctypes.c_void_p(host_addr),
        buf,
        size,
        ctypes.byref(bytes_read)
    )
    if not ok:
        return None
    return bytes(buf)[:bytes_read.value]


def read_full_eemem(proc_handle, eemem_base, eemem_size):
    """Read the entire 32MB PS2 RAM in chunks."""
    chunk_size = 4 * 1024 * 1024  # 4MB chunks
    full_ram = bytearray()
    
    for offset in range(0, eemem_size, chunk_size):
        remaining = min(chunk_size, eemem_size - offset)
        chunk = read_eemem(proc_handle, eemem_base, offset, remaining)
        if chunk is None:
            print(f"    [-] Failed to read chunk at PS2 offset 0x{offset:08X}")
            full_ram.extend(b'\x00' * remaining)
        else:
            full_ram.extend(chunk)
            if len(chunk) < remaining:
                full_ram.extend(b'\x00' * (remaining - len(chunk)))
        
        progress = (offset + remaining) * 100 // eemem_size
        print(f"\r    Reading EEmem... {progress}%", end='', flush=True)
    
    print(f"\r    Reading EEmem... 100% ({len(full_ram):,} bytes captured)")
    return bytes(full_ram)

# ─────────────────────────────────────────────────────────────────────────────
# Hash Scanning Engine
# ─────────────────────────────────────────────────────────────────────────────

def scan_for_hashes(ram_data, target_hashes):
    """
    Scan the full 32MB RAM dump for 4-byte LE occurrences of each target hash.
    Returns a list of (hash_value, ps2_offset) tuples.
    """
    results = []
    
    for h in target_hashes:
        needle = struct.pack('<I', h)
        search_start = 0
        occurrences = []
        
        while True:
            idx = ram_data.find(needle, search_start)
            if idx == -1:
                break
            occurrences.append(idx)
            search_start = idx + 1
        
        if occurrences:
            print(f"  [+] Hash 0x{h:08X}: {len(occurrences)} occurrence(s) at "
                  f"PS2 offsets: {', '.join(f'0x{o:08X}' for o in occurrences[:8])}"
                  f"{'...' if len(occurrences) > 8 else ''}")
            for occ in occurrences:
                results.append((h, occ))
        else:
            print(f"  [-] Hash 0x{h:08X}: NOT FOUND in live RAM")
    
    return results

# ─────────────────────────────────────────────────────────────────────────────
# Hex Dump Formatter
# ─────────────────────────────────────────────────────────────────────────────

def format_hexdump(data, base_offset, highlight_offset=None, highlight_len=4):
    """
    Format binary data into a clean hex/ASCII dump.
    Optionally highlights a specific offset range (the hash match).
    """
    lines = []
    for row_start in range(0, len(data), 16):
        row_data = data[row_start:row_start + 16]
        addr = base_offset + row_start
        
        # Hex part
        hex_parts = []
        for i, b in enumerate(row_data):
            abs_pos = row_start + i
            # Mark the matched hash bytes with brackets
            if (highlight_offset is not None and
                highlight_offset <= abs_pos < highlight_offset + highlight_len):
                hex_parts.append(f"[{b:02X}]")
            else:
                hex_parts.append(f" {b:02X} ")
        
        hex_str = ''.join(hex_parts)
        # Pad if row is less than 16 bytes
        if len(row_data) < 16:
            hex_str += '    ' * (16 - len(row_data))
        
        # ASCII part
        ascii_parts = []
        for b in row_data:
            if 32 <= b < 127:
                ascii_parts.append(chr(b))
            else:
                ascii_parts.append('.')
        ascii_str = ''.join(ascii_parts)
        
        lines.append(f"  {addr:08X}: {hex_str}  |{ascii_str}|")
    
    return '\n'.join(lines)

# ─────────────────────────────────────────────────────────────────────────────
# Contextual Structure Analysis
# ─────────────────────────────────────────────────────────────────────────────

def analyze_context(ram_data, hash_val, ps2_offset):
    """
    Analyze the memory region around a found hash for structural clues.
    Look for FJBO magic, node headers, pointer patterns, etc.
    """
    analysis = []
    
    # Check for FJBO magic in the surrounding area (±4KB)
    search_start = max(0, ps2_offset - 4096)
    search_end = min(len(ram_data), ps2_offset + 4096)
    region = ram_data[search_start:search_end]
    
    fjbo_idx = region.find(b'FJBO')
    if fjbo_idx >= 0:
        fjbo_ps2_addr = search_start + fjbo_idx
        analysis.append(f"  FJBO magic found at PS2 offset 0x{fjbo_ps2_addr:08X} "
                       f"({ps2_offset - fjbo_ps2_addr:+d} bytes from hash)")
    
    # Read surrounding 32-bit values to look for node type IDs
    interesting_types = {
        0x72700: "Frontiers Character Root",
        0x62700: "Vanilla Character Root",
        0x42710: "Metadata/BBox",
        0x11110: "Texture Container",
        0x02800: "Skeleton Joints",
        0x02610: "Mesh Container",
        0x22400: "Frontiers Bone Defs",
        0x12400: "Vanilla Bone Defs",
        0x0A010: "Model Container",
        0x02950: "Frontiers Trailer 1",
        0x02960: "Frontiers Trailer 2",
    }
    
    # Scan 256 bytes around the hash for known type IDs
    scan_start = max(0, ps2_offset - 256)
    scan_end = min(len(ram_data), ps2_offset + 256)
    
    for off in range(scan_start, scan_end - 3, 4):
        val = struct.unpack_from('<I', ram_data, off)[0]
        if val in interesting_types:
            rel = off - ps2_offset
            analysis.append(f"  Known type 0x{val:05X} ({interesting_types[val]}) "
                          f"at PS2 0x{off:08X} ({rel:+d} from hash)")
    
    # Check if the hash sits inside what looks like a pointer table entry
    # Typical pointer table entry: [offset_32, length_32, hash_32, type_32]
    if ps2_offset >= 8:
        pre_vals = struct.unpack_from('<II', ram_data, ps2_offset - 8)
        post_val = struct.unpack_from('<I', ram_data, ps2_offset + 4)[0] if ps2_offset + 8 <= len(ram_data) else 0
        analysis.append(f"  Preceding 2 DWORDs: 0x{pre_vals[0]:08X}, 0x{pre_vals[1]:08X}")
        analysis.append(f"  Following DWORD:    0x{post_val:08X}")
        
        # If preceding values look like offset+length (reasonable ranges)
        if pre_vals[0] < 0x10000000 and pre_vals[1] < 0x01000000:
            analysis.append(f"  >> Possible pointer table entry: "
                          f"offset=0x{pre_vals[0]:08X}, length={pre_vals[1]:,}")
    
    return analysis

# ─────────────────────────────────────────────────────────────────────────────
# FJBO / Node Tree Live Scanner
# ─────────────────────────────────────────────────────────────────────────────

def scan_for_loaded_models(ram_data):
    """
    Scan the entire live RAM for FJBO magic bytes to find all loaded ESF
    databases and individual model containers in memory.
    """
    results = []
    search_start = 0
    
    while True:
        idx = ram_data.find(b'FJBO', search_start)
        if idx == -1:
            break
        
        # Read the ESF header
        if idx + 32 <= len(ram_data):
            hdr = ram_data[idx:idx+32]
            magic = hdr[0:4]
            version = struct.unpack_from('<I', hdr, 4)[0]
            constant = struct.unpack_from('<I', hdr, 8)[0]
            hdr_size = struct.unpack_from('<I', hdr, 16)[0]
            
            results.append({
                'offset': idx,
                'version': version,
                'constant': constant,
                'header_size': hdr_size,
            })
        
        search_start = idx + 4
    
    return results


def scan_for_node_types(ram_data, type_id, max_results=50):
    """Scan RAM for 12-byte node headers with a specific type_id."""
    needle = struct.pack('<I', type_id)
    results = []
    search_start = 0
    
    while len(results) < max_results:
        idx = ram_data.find(needle, search_start)
        if idx == -1:
            break
        
        # Validate: next 8 bytes should be data_size and child_count
        if idx + 12 <= len(ram_data):
            data_size = struct.unpack_from('<I', ram_data, idx + 4)[0]
            child_count = struct.unpack_from('<I', ram_data, idx + 8)[0]
            
            # Sanity checks: data_size < 10MB, child_count < 100
            if data_size < 10_000_000 and child_count < 100:
                results.append({
                    'offset': idx,
                    'type_id': type_id,
                    'data_size': data_size,
                    'child_count': child_count,
                })
        
        search_start = idx + 4
    
    return results

# ─────────────────────────────────────────────────────────────────────────────
# Main Orchestration
# ─────────────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="EQOA Live RAM Tracer — Dynamic Runtime Memory Analysis for PCSX2"
    )
    parser.add_argument('--hash', type=str, default=None,
                       help='Specific hash to search for (hex, e.g., 0x05AEBA67). '
                            'If not specified, searches for all 11 target hashes.')
    parser.add_argument('--output', type=str, default='diagnostics/logs/live_memory_dump.txt',
                       help='Output file path for hex dumps.')
    parser.add_argument('--scan-models', action='store_true',
                       help='Also scan for all FJBO databases and model node types in RAM.')
    parser.add_argument('--continuous', action='store_true',
                       help='Run in continuous mode, re-scanning every 5 seconds.')
    parser.add_argument('--duration', type=int, default=None,
                       help='In continuous mode, exit after this many seconds.')
    args = parser.parse_args()

    print("=" * 74)
    print("  EQOA LIVE RAM TRACER — Dynamic Runtime Memory Analysis")
    print("  Targeting: PCSX2 Emotion Engine Main RAM (32MB)")
    print("=" * 74)
    print()

    # ── Step 1: Find PCSX2 process ──────────────────────────────────────────
    pid_and_name = find_pcsx2_pid()
    if pid_and_name is None or pid_and_name[0] is None:
        sys.exit(1)
    pid, exe_name = pid_and_name

    # ── Step 2: Open process handle ─────────────────────────────────────────
    proc_handle = open_process(pid)
    if proc_handle is None:
        sys.exit(1)

    try:
        # ── Step 3: Locate EEmem ────────────────────────────────────────────
        eemem_base, eemem_size = find_eemem(proc_handle, pid, exe_name)
        if eemem_base is None:
            sys.exit(1)

        start_time = time.time()
        scan_count = 0
        while True:
            scan_count += 1
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            # ── Step 4: Read full EEmem ─────────────────────────────────────
            print(f"\n[*] [{timestamp}] Capturing full EEmem snapshot...")
            ram_data = read_full_eemem(proc_handle, eemem_base, eemem_size)

            if len(ram_data) != eemem_size:
                print(f"[-] WARNING: Read {len(ram_data):,} bytes, expected {eemem_size:,}")

            # ── Step 5: Scan for target hashes ──────────────────────────────
            if args.hash:
                search_hashes = [int(args.hash, 16)]
            else:
                search_hashes = TARGET_HASHES

            print(f"\n[*] Scanning live RAM for {len(search_hashes)} target hash(es)...")
            hits = scan_for_hashes(ram_data, search_hashes)

            # ── Step 6: Scan for loaded models (optional) ───────────────────
            fjbo_entries = []
            model_nodes_72700 = []
            model_nodes_62700 = []

            if args.scan_models:
                print(f"\n[*] Scanning for FJBO databases in live RAM...")
                fjbo_entries = scan_for_loaded_models(ram_data)
                print(f"    Found {len(fjbo_entries)} FJBO database(s) in memory:")
                for entry in fjbo_entries:
                    print(f"      PS2 0x{entry['offset']:08X}: "
                          f"ver={entry['version']}, const=0x{entry['constant']:04X}, "
                          f"hdr_size={entry['header_size']}")

                print(f"\n[*] Scanning for Frontiers character roots (0x72700)...")
                model_nodes_72700 = scan_for_node_types(ram_data, 0x72700)
                print(f"    Found {len(model_nodes_72700)} node(s):")
                for node in model_nodes_72700:
                    print(f"      PS2 0x{node['offset']:08X}: "
                          f"data_size={node['data_size']:,}, children={node['child_count']}")

                print(f"\n[*] Scanning for Vanilla character roots (0x62700)...")
                model_nodes_62700 = scan_for_node_types(ram_data, 0x62700)
                print(f"    Found {len(model_nodes_62700)} node(s):")
                for node in model_nodes_62700:
                    print(f"      PS2 0x{node['offset']:08X}: "
                          f"data_size={node['data_size']:,}, children={node['child_count']}")

            # ── Step 7: Generate hex dumps (APPEND to log) ────────────────
            if args.output.lower() == 'stdout':
                class DummyContext:
                    def __enter__(self): return sys.stdout
                    def __exit__(self, exc_type, exc_val, exc_tb): pass
                ctx = DummyContext()
            else:
                os.makedirs(os.path.dirname(args.output) or '.', exist_ok=True)
                ctx = open(args.output, 'a')
                
            with ctx as out:
                out.write("\n" + "=" * 78 + "\n")
                out.write(f"  SCAN #{scan_count} — {timestamp}\n")
                out.write(f"  PCSX2 PID: {pid} ({exe_name})\n")
                out.write(f"  EEmem Base (Host): 0x{eemem_base:016X}\n")
                out.write(f"  EEmem Size: {eemem_size:,} bytes\n")
                if DIAGNOSTIC_LOG:
                    out.write("  EE RAM Discovery Log:\n")
                    for line in DIAGNOSTIC_LOG:
                        out.write(f"    {line}\n")
                out.write("=" * 78 + "\n\n")

                if not hits:
                    out.write("[!] NO TARGET HASHES FOUND IN LIVE RAM.\n")
                    out.write("    This means the game engine has NOT loaded our patched\n")
                    out.write("    character models into EE memory, OR it loaded and then\n")
                    out.write("    discarded/overwritten them.\n\n")
                    out.write("    Possible causes:\n")
                    out.write("    1. The IOP CDVD driver failed to read CHAR.ESF from the patched LBA.\n")
                    out.write("    2. The engine loaded the original CHAR.ESF from a cached/duplicate location.\n")
                    out.write("    3. The asset hash is stored differently in the runtime entity table.\n")
                    out.write("    4. The character has not spawned yet (try moving/logging in).\n\n")

                for hash_val, ps2_offset in hits:
                    out.write("-" * 78 + "\n")
                    out.write(f"  HASH MATCH: 0x{hash_val:08X} at PS2 offset 0x{ps2_offset:08X}\n")
                    out.write(f"  Host address: 0x{eemem_base + ps2_offset:016X}\n")
                    out.write("-" * 78 + "\n\n")

                    # Context analysis
                    context = analyze_context(ram_data, hash_val, ps2_offset)
                    if context:
                        out.write("  Structural Context Analysis:\n")
                        for line in context:
                            out.write(f"  {line}\n")
                        out.write("\n")

                    # Compute dump boundaries
                    dump_start = max(0, ps2_offset - DUMP_BEFORE)
                    dump_end = min(len(ram_data), ps2_offset + DUMP_AFTER)
                    dump_data = ram_data[dump_start:dump_end]
                    highlight_in_dump = ps2_offset - dump_start

                    out.write(f"  Memory Dump: PS2 0x{dump_start:08X} — 0x{dump_end:08X} "
                             f"({dump_end - dump_start:,} bytes)\n")
                    out.write(f"  Hash location marked with [XX] brackets\n\n")
                    out.write(format_hexdump(dump_data, dump_start,
                                           highlight_in_dump, 4))
                    out.write("\n\n")

                # FJBO scan results
                if fjbo_entries:
                    out.write("=" * 78 + "\n")
                    out.write("  FJBO DATABASE INSTANCES IN LIVE RAM\n")
                    out.write("=" * 78 + "\n\n")
                    for entry in fjbo_entries:
                        out.write(f"  PS2 0x{entry['offset']:08X}: "
                                 f"version={entry['version']}, "
                                 f"constant=0x{entry['constant']:04X}, "
                                 f"header_size={entry['header_size']}\n")

                        # Dump first 128 bytes of each FJBO
                        fjbo_data = ram_data[entry['offset']:
                                           min(entry['offset'] + 128, len(ram_data))]
                        out.write(format_hexdump(fjbo_data, entry['offset']))
                        out.write("\n\n")

                # Model node scan results
                if model_nodes_72700 or model_nodes_62700:
                    out.write("=" * 78 + "\n")
                    out.write("  CHARACTER MODEL ROOT NODES IN LIVE RAM\n")
                    out.write("=" * 78 + "\n\n")

                    for label, nodes in [("0x72700 (Frontiers)", model_nodes_72700),
                                        ("0x62700 (Vanilla)", model_nodes_62700)]:
                        out.write(f"  Type {label}: {len(nodes)} instance(s)\n")
                        for node in nodes:
                            out.write(f"    PS2 0x{node['offset']:08X}: "
                                     f"data_size={node['data_size']:,}, "
                                     f"children={node['child_count']}\n")
                            # Dump 64 bytes at the node header
                            node_data = ram_data[node['offset']:
                                               min(node['offset'] + 64, len(ram_data))]
                            out.write(format_hexdump(node_data, node['offset']))
                            out.write("\n")
                        out.write("\n")

                out.write(f"\n  --- END SCAN #{scan_count} ---\n\n")

            if args.output.lower() != 'stdout':
                abs_out = os.path.abspath(args.output)
                print(f"\n{'=' * 74}")
                print(f"  SCAN #{scan_count} COMPLETE — {len(hits)} hash match(es) found")
                print(f"  Appended to log: {abs_out}")
                print(f"{'=' * 74}")
            else:
                print(f"\n{'=' * 74}")
                print(f"  SCAN #{scan_count} COMPLETE — {len(hits)} hash match(es) found")
                print(f"{'=' * 74}")

            if not args.continuous:
                break

            if args.duration and (time.time() - start_time) >= args.duration:
                print(f"\n[*] Duration limit of {args.duration}s reached. Exiting continuous mode.")
                break

            print(f"\n[*] Continuous mode: next scan in 5 seconds... (Ctrl+C to stop)")
            try:
                time.sleep(5)
            except KeyboardInterrupt:
                print("\n[*] Stopped by user.")
                break

    finally:
        kernel32.CloseHandle(proc_handle)
        print("[*] Process handle released. Done.")


if __name__ == '__main__':
    main()
