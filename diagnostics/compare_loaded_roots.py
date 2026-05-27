import sys
import os
import struct
import ctypes
import ctypes.wintypes

PROCESS_ALL_ACCESS = 0x001F0FFF
PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400
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

# ─────────────────────────────────────────────────────────────────────────────
# Helper Memory Reading & Module Resolution Functions
# ─────────────────────────────────────────────────────────────────────────────

def find_pcsx2_pid():
    target_names = [b"pcsx2.exe", b"pcsx2-qt.exe", b"pcsx2x64.exe",
                    b"pcsx2-qtx64.exe", b"pcsx2-avx2.exe"]
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    if snapshot == -1 or snapshot == ctypes.c_void_p(-1).value:
        return None, None
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
            if found_pid or not kernel32.Process32Next(snapshot, ctypes.byref(pe)):
                break
    kernel32.CloseHandle(snapshot)
    return found_pid, found_name

def get_module_base(pid, exe_name):
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)
    if snapshot == -1 or snapshot == ctypes.c_void_p(-1).value:
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

def find_eemem_base(proc_handle, pid, exe_name):
    base_addr = get_module_base(pid, exe_name)
    if not base_addr:
        return None
    dos_hdr = read_remote_mem(proc_handle, base_addr, 64)
    if not dos_hdr or dos_hdr[:2] != b'MZ':
        return None
    pe_offset = struct.unpack_from('<I', dos_hdr, 0x3c)[0]
    pe_hdr = read_remote_mem(proc_handle, base_addr + pe_offset, 264)
    if not pe_hdr or pe_hdr[:4] != b'PE\x00\x00':
        return None
    magic = struct.unpack_from('<H', pe_hdr, 24)[0]
    is_64 = (magic == 0x20B)
    export_dir_offset = 24 + 112 if is_64 else 24 + 96
    export_rva, _ = struct.unpack_from('<II', pe_hdr, export_dir_offset)
    if export_rva == 0:
        return None
    export_dir_bytes = read_remote_mem(proc_handle, base_addr + export_rva, 40)
    if not export_dir_bytes:
        return None
    num_names = struct.unpack_from('<I', export_dir_bytes, 24)[0]
    funcs_rva = struct.unpack_from('<I', export_dir_bytes, 28)[0]
    names_rva = struct.unpack_from('<I', export_dir_bytes, 32)[0]
    ords_rva = struct.unpack_from('<I', export_dir_bytes, 36)[0]
    
    names_data = read_remote_mem(proc_handle, base_addr + names_rva, num_names * 4)
    ords_data = read_remote_mem(proc_handle, base_addr + ords_rva, num_names * 2)
    funcs_data = read_remote_mem(proc_handle, base_addr + funcs_rva, num_names * 4)
    
    if not names_data or not ords_data or not funcs_data:
        return None
        
    for i in range(num_names):
        name_rva = struct.unpack_from('<I', names_data, i * 4)[0]
        name_bytes = read_remote_mem(proc_handle, base_addr + name_rva, 64)
        if not name_bytes:
            continue
        name_str = name_bytes.split(b'\x00')[0].decode('utf-8', errors='replace')
        if name_str == "EEmem":
            ord_val = struct.unpack_from('<H', ords_data, i * 2)[0]
            func_rva = struct.unpack_from('<I', funcs_data, ord_val * 4)[0]
            eemem_ptr = base_addr + func_rva
            ptr_bytes = read_remote_mem(proc_handle, eemem_ptr, 8)
            if ptr_bytes:
                return struct.unpack('<Q', ptr_bytes)[0]
    return None

# ─────────────────────────────────────────────────────────────────────────────
# ESF Node Tree Parsing
# ─────────────────────────────────────────────────────────────────────────────

import sys
sys.setrecursionlimit(10000)

def parse_esf_node(data, pos=0, depth=0, max_depth=25):
    """Recursively parses a standalone ESF node from raw memory bytes contiguously with guards against infinite recursion."""
    if depth > max_depth:
        return None, pos
        
    if pos + 12 > len(data):
        return None, pos
        
    type_id = struct.unpack_from('<I', data, pos)[0]
    data_size = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    
    # Sanity checks: reject obvious garbage memory parsing
    if child_count > 250 or data_size > len(data) or type_id > 0xFFFFFF:
        return None, pos + 12
        
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
        # Leaf node
        node['inline_data'] = data[next_pos:min(next_pos + data_size, len(data))]
    else:
        # Branch node: recursively parse exactly child_count children
        for _ in range(child_count):
            if next_pos + 12 > len(data):
                break
            child, next_pos = parse_esf_node(data, next_pos, depth + 1, max_depth)
            if child:
                node['children'].append(child)
                
    return node, pos + 12 + data_size

def dump_node_tree(node, depth=0, max_depth=5):
    """Formats a node tree as a clean string visualization."""
    indent = "  " * depth
    type_hex = f"0x{node['type_id']:05X}"
    
    # Check for known types names
    known_names = {
        0x72700: "Frontiers Character Root",
        0x62700: "Vanilla Character Root",
        0x42710: "Metadata/BBox",
        0x11110: "Texture Container",
        0x11111: "Texture Asset Leaf",
        0x00001001: "Texture Data Payload",
        0x02800: "Skeleton Joints Container",
        0x22400: "Frontiers Bone Defs Container",
        0x12400: "Vanilla Bone Defs Container",
        0x02610: "Mesh Container",
        0x0A010: "Model/Submesh Container",
        0x02950: "Frontiers Trailer 1",
        0x02960: "Frontiers Trailer 2",
    }
    
    name = known_names.get(node['type_id'], f"UnknownNode_{node['type_id']:X}")
    result = f"{indent}- {name} ({type_hex}): size={node['data_size']:,}, children={node['child_count']}"
    
    # Print asset ID leaf values
    if node['type_id'] == 0x11111 and node['inline_data'] and len(node['inline_data']) >= 4:
        val = struct.unpack('<I', node['inline_data'][:4])[0]
        result += f" -> Hash = 0x{val:08X}"
        
    lines = [result]
    if depth < max_depth:
        for child in node['children']:
            lines.extend(dump_node_tree(child, depth + 1, max_depth))
    elif node['children']:
        lines.append(f"{indent}  ... (max depth reached, {len(node['children'])} children truncated)")
        
    return lines

# ─────────────────────────────────────────────────────────────────────────────
# Main Execution
# ─────────────────────────────────────────────────────────────────────────────

def main():
    print("=" * 78)
    # ── Step 1: Attach to PCSX2 and resolve EEmem ───────────────────────────
    pid, name = find_pcsx2_pid()
    if not pid:
        print("[-] PCSX2 not found.")
        return
        
    hProcess = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    if not hProcess:
        hProcess = kernel32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, pid)
    if not hProcess:
        print("[-] Failed to open process.")
        return
        
    eemem_base = find_eemem_base(hProcess, pid, name)
    if not eemem_base:
        print("[-] Failed to resolve EEmem base.")
        kernel32.CloseHandle(hProcess)
        return
        
    print(f"[+] Connected to {name} (PID: {pid})")
    print(f"[+] EEmem Base Address: 0x{eemem_base:X}")
    print("=" * 78)
    
    # ── Step 2: Read Full EE RAM & Scan Dynamically for Character Roots ─────
    print("\n[*] Capturing active EE RAM snapshot (32MB)...")
    
    def read_full_eemem(proc_handle, eemem_base, eemem_size=0x02000000):
        chunk_size = 4 * 1024 * 1024
        full_ram = bytearray()
        for offset in range(0, eemem_size, chunk_size):
            remaining = min(chunk_size, eemem_size - offset)
            buf = ctypes.create_string_buffer(remaining)
            bytes_read = ctypes.c_size_t(0)
            ok = kernel32.ReadProcessMemory(
                proc_handle,
                ctypes.c_void_p(eemem_base + offset),
                buf,
                remaining,
                ctypes.byref(bytes_read)
            )
            if not ok:
                full_ram.extend(b'\x00' * remaining)
            else:
                full_ram.extend(buf.raw[:bytes_read.value])
                if bytes_read.value < remaining:
                    full_ram.extend(b'\x00' * (remaining - bytes_read.value))
        return bytes(full_ram)

    ram_data = read_full_eemem(hProcess, eemem_base)
    print(f"[+] Snapshot captured: {len(ram_data):,} bytes")

    # Scan for 0x72700 roots
    print("[*] Scanning for character root nodes (0x72700)...")
    needle = struct.pack('<I', 0x72700)
    roots = []
    search_start = 0
    while True:
        idx = ram_data.find(needle, search_start)
        if idx == -1:
            break
        if idx + 12 <= len(ram_data):
            data_size = struct.unpack_from('<I', ram_data, idx + 4)[0]
            child_count = struct.unpack_from('<I', ram_data, idx + 8)[0]
            if child_count == 17 and data_size < 10_000_000:
                roots.append((idx, data_size))
        search_start = idx + 4

    print(f"[+] Found {len(roots)} active character roots in RAM:")
    ref_offset = None
    pat_offset = None
    ref_size = 457095
    # Patched size is usually 505931, but we can search for the highest size or close matches
    for idx, data_size in roots:
        print(f"  - PS2 Offset 0x{idx:08X}: data_size={data_size:,} bytes")
        if data_size == ref_size:
            ref_offset = idx
        elif data_size > 480000 and (pat_offset is None or abs(data_size - 505931) < abs(ram_data.find(b'FJBO') - 505931)): # or closest to 505931
            # Pick a node with size > 480KB (which would be our patched node, other unpatched nodes are <460KB or around 501KB)
            # Actually, let's identify the patched one:
            # According to the tracer:
            # 0x00E2ECC8: data_size=501,461, children=17
            # 0x00EA93C9: data_size=494,666, children=17
            # 0x00F2203F: data_size=507,084, children=17
            # 0x00F9DD37: data_size=505,931, children=17
            # Let's match exact 505931 or closest
            if pat_offset is None:
                pat_offset = idx
            else:
                # If we have multiple, the one with size 505931 is preferred
                current_best_diff = abs(ram_data.find(needle) - 505931) # dummy
                if data_size == 505931:
                    pat_offset = idx
                elif abs(data_size - 505931) < 1000:
                    pat_offset = idx

    # If ref_offset or pat_offset is not found, try heuristics
    if not ref_offset and roots:
        # Reference is typically the smallest active root that is non-zero
        valid_ref = [r for r in roots if r[1] > 400000 and r[1] < 460000]
        if valid_ref:
            ref_offset = valid_ref[0][0]
            ref_size = valid_ref[0][1]

    if not pat_offset and roots:
        # Patched is typically the one with size around 505931
        valid_pat = [r for r in roots if abs(r[1] - 505931) < 5000]
        if valid_pat:
            pat_offset = valid_pat[0][0]
            pat_size = valid_pat[0][1]
        else:
            # Fallback to the largest node
            roots_sorted = sorted(roots, key=lambda x: x[1], reverse=True)
            if roots_sorted:
                pat_offset = roots_sorted[0][0]
                pat_size = roots_sorted[0][1]

    if not ref_offset or not pat_offset:
        print("[-] Could not dynamically identify the Reference and Patched root offsets.")
        print("    Make sure the patched character is loaded and spawned in the world.")
        kernel32.CloseHandle(hProcess)
        return

    ref_size = next(size for idx, size in roots if idx == ref_offset)
    pat_size = next(size for idx, size in roots if idx == pat_offset)

    print(f"\n[+] Dynamic Resolution Success:")
    print(f"    - Working Reference Root: PS2 0x{ref_offset:08X} (size={ref_size:,})")
    print(f"    - Patched Invisible Root: PS2 0x{pat_offset:08X} (size={pat_size:,})")

    print(f"\n[*] Reading active RAM for Working Reference Root (0x{ref_offset:08X})...")
    ref_raw = read_remote_mem(hProcess, eemem_base + ref_offset, ref_size + 12)
    
    print(f"[*] Reading active RAM for Patched Invisible Root (0x{pat_offset:08X})...")
    pat_raw = read_remote_mem(hProcess, eemem_base + pat_offset, pat_size + 12)
    
    kernel32.CloseHandle(hProcess)
    
    if not ref_raw or not pat_raw:
        print("[-] Failed to read model memory blocks from emulator RAM.")
        return
        
    # ── Step 3: Parse Node Trees recursively ────────────────────────────────
    print("\n[*] Recursively parsing working reference node tree...")
    ref_tree, _ = parse_esf_node(ref_raw)
    
    print("[*] Recursively parsing patched invisible node tree...")
    pat_tree, _ = parse_esf_node(pat_raw)
    
    if not ref_tree or not pat_tree:
        print("[-] Tree parsing failed.")
        return
        
    # ── Step 4: Compare Trees ───────────────────────────────────────────────
    report = []
    report.append("=" * 80)
    report.append("  DYNAMIC MEMORY NODE TREE COMPARISON REPORT")
    report.append(f"  Working Reference: PS2 0x{ref_offset:08X} | Patched Invisible: PS2 0x{pat_offset:08X}")
    report.append("=" * 80)
    report.append("")
    
    report.append(f"[+] working reference tree (0x{ref_offset:08X}):")
    report.extend(dump_node_tree(ref_tree, 1, 6))
    report.append("")
    
    report.append(f"[+] patched invisible tree (0x{pat_offset:08X}):")
    report.extend(dump_node_tree(pat_tree, 1, 6))
    report.append("")
    
    # granular comparison of first level subnodes
    report.append("=" * 80)
    report.append("  SUB-COMPONENT DETAILED COMPARISON")
    report.append("=" * 80)
    
    ref_children = ref_tree['children']
    pat_children = pat_tree['children']
    
    report.append(f"Working Reference children count: {len(ref_children)}")
    report.append(f"Patched Invisible children count: {len(pat_children)}")
    report.append("")
    
    # We want to identify the types and sizes of subcomponents
    # Commonly:
    # Child 0: Metadata/BBox (0x42710)
    # Child 1: Skeleton/Bones (0x02800 or 0x22400)
    # Child 2: Mesh Container (0x02610)
    # Child 3: Texture Container (0x11110)
    # etc...
    
    report.append(f"{'Index':<6} | {'Ref Component Type':<30} (Size) | {'Pat Component Type':<30} (Size) | Status")
    report.append("-" * 90)
    
    max_children = max(len(ref_children), len(pat_children))
    for i in range(max_children):
        ref_comp_str = "None"
        pat_comp_str = "None"
        status = "MATCH"
        
        ref_child = ref_children[i] if i < len(ref_children) else None
        pat_child = pat_children[i] if i < len(pat_children) else None
        
        if ref_child:
            type_hex = f"0x{ref_child['type_id']:05X}"
            ref_comp_str = f"{type_hex} ({ref_child['data_size']:,} bytes)"
        if pat_child:
            type_hex = f"0x{pat_child['type_id']:05X}"
            pat_comp_str = f"{type_hex} ({pat_child['data_size']:,} bytes)"
            
        if not ref_child or not pat_child:
            status = "MISMATCH"
        elif ref_child['type_id'] != pat_child['type_id']:
            status = "MISMATCH"
            
        report.append(f"{i:<6} | {ref_comp_str:<36} | {pat_comp_str:<36} | {status}")
        
    report.append("")
    
    # ── Step 5: Advanced Sub-Component Structural Inspection ────────────────
    # Find Skeleton and Mesh containers for deeper inspection
    def find_subnode(node, type_id):
        if node['type_id'] == type_id:
            return node
        for child in node['children']:
            found = find_subnode(child, type_id)
            if found:
                return found
        return None
        
    ref_skel = find_subnode(ref_tree, 0x02800) or find_subnode(ref_tree, 0x22400) or find_subnode(ref_tree, 0x12400)
    pat_skel = find_subnode(pat_tree, 0x02800) or find_subnode(pat_tree, 0x22400) or find_subnode(pat_tree, 0x12400)
    
    report.append("=" * 80)
    report.append("  SKELETON & RIGGING NODE AUDIT")
    report.append("=" * 80)
    if ref_skel and pat_skel:
        report.append(f"Working Ref skeleton type:  0x{ref_skel['type_id']:05X} | size: {ref_skel['data_size']:,} | children: {ref_skel['child_count']}")
        report.append(f"Patched Invisible skel type:0x{pat_skel['type_id']:05X} | size: {pat_skel['data_size']:,} | children: {pat_skel['child_count']}")
        if ref_skel['type_id'] != pat_skel['type_id']:
            report.append("  [!] WARNING: Skeleton type mismatch! working uses 0x{:05X}, patched uses 0x{:05X}".format(ref_skel['type_id'], pat_skel['type_id']))
        if ref_skel['child_count'] != pat_skel['child_count']:
            report.append("  [!] WARNING: Bone count mismatch! Working: {}, Patched: {}".format(ref_skel['child_count'], pat_skel['child_count']))
    else:
        report.append(f"Skeleton found in Working Ref:  {'YES' if ref_skel else 'NO'}")
        report.append(f"Skeleton found in Patched:      {'YES' if pat_skel else 'NO'}")
        
    ref_mesh = find_subnode(ref_tree, 0x02610)
    pat_mesh = find_subnode(pat_tree, 0x02610)
    
    report.append("")
    report.append("=" * 80)
    report.append("  GEOMETRY & MESH NODE AUDIT")
    report.append("=" * 80)
    if ref_mesh and pat_mesh:
        report.append(f"Working Ref mesh container:  size: {ref_mesh['data_size']:,} | submeshes: {ref_mesh['child_count']}")
        report.append(f"Patched Invisible mesh cont: size: {pat_mesh['data_size']:,} | submeshes: {pat_mesh['child_count']}")
        
        # Details of each submesh
        report.append("\n  Submesh layouts:")
        for idx, (r_child, p_child) in enumerate(zip(ref_mesh['children'], pat_mesh['children'])):
            report.append(f"    Submesh {idx}:")
            report.append(f"      Ref: Type 0x{r_child['type_id']:05X} | size {r_child['data_size']:,} | children {r_child['child_count']}")
            report.append(f"      Pat: Type 0x{p_child['type_id']:05X} | size {p_child['data_size']:,} | children {p_child['child_count']}")
    else:
        report.append(f"Mesh Container found in Working Ref:  {'YES' if ref_mesh else 'NO'}")
        report.append(f"Mesh Container found in Patched:      {'YES' if pat_mesh else 'NO'}")
        
    report.append("")
    report.append("=" * 80)
    report.append("  CONCLUSION & DIAGNOSIS SUGGESTIONS")
    report.append("=" * 80)
    
    # Analyze if there's structural issues
    if ref_skel and pat_skel and ref_mesh and pat_mesh:
        if ref_skel['type_id'] == 0x22400 and pat_skel['type_id'] != 0x22400:
            report.append("  [CRITICAL] Frontiers engine requires 0x22400 bones nodes for high-level skeletons.")
            report.append("             Patched model is using 0x{:05X} which is a legacy/Vanilla structure type!".format(pat_skel['type_id']))
            report.append("             This triggers structural parsing failures in the Frontiers skeletal anim compiler.")
        elif ref_skel['child_count'] != pat_skel['child_count']:
            report.append("  [CRITICAL] Bone count mismatch: Working has {}, Patched has {}.".format(ref_skel['child_count'], pat_skel['child_count']))
            report.append("             This causes VU1 vector processing to collapse because mesh vertex weights reference non-existent bones!")
        else:
            report.append("  [INFO] Skeletal containers match structurally. Check vertex buffer layout & textures link.")
    
    report_content = "\n".join(report)
    print(report_content)
    
    # Save report
    os.makedirs("diagnostics/logs", exist_ok=True)
    with open("diagnostics/logs/comparison_report.txt", "w") as f:
        f.write(report_content + "\n")
    print("\n[+] Detailed comparison report saved to diagnostics/logs/comparison_report.txt")

if __name__ == '__main__':
    main()
