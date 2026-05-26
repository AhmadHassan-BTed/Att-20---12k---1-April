import sys
import os
import struct
import ctypes
import ctypes.wintypes

sys.path.append(r't:\Att 20 - 12k - 1 April')

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

# Correct parser
def parse_node(data: bytes, pos: int) -> tuple:
    if pos + 12 > len(data):
        return None, pos
    type_id     = struct.unpack_from('<I', data, pos    )[0]
    data_size   = struct.unpack_from('<I', data, pos + 4)[0]
    child_count = struct.unpack_from('<I', data, pos + 8)[0]
    node = {
        'type_id': type_id, 'data_size': data_size,
        'child_count': child_count, 'children': [], 'inline_data': None,
        'pos': pos
    }
    pos += 12
    if child_count == 0:
        if pos + data_size > len(data):
            # Safe slice
            node['inline_data'] = data[pos : len(data)]
            pos = len(data)
        else:
            node['inline_data'] = data[pos : pos + data_size]
            pos += data_size
    else:
        for _ in range(child_count):
            child, pos = parse_node(data, pos)
            if child is not None:
                node['children'].append(child)
    return node, pos

def dump_tree(node, depth=0):
    indent = "  " * depth
    print(f"{indent}- Type 0x{node['type_id']:05X} (size={node['data_size']:,}, children={node['child_count']})")
    for child in node['children']:
        dump_tree(child, depth + 1)

def main():
    pid, name = find_pcsx2_pid()
    if not pid:
        print("[-] PCSX2 not found.")
        return
    hProcess = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    if not hProcess:
        print("[-] Failed to open process.")
        return
    eemem_base = find_eemem_base(hProcess, pid, name)
    if not eemem_base:
        print("[-] Failed to resolve EEmem base.")
        return
        
    print(f"[+] Connected to {name} (PID: {pid})")
    print(f"[+] EEmem Base Address: 0x{eemem_base:X}")
    
    # Let's read the reference at 0x001B6BC8 and patched at 0x00F9DD37
    # According to diagnostic log:
    ref_offset = 0x001B6BC8
    ref_size = 453597
    pat_offset = 0x00F9DD37
    pat_size = 505931
    
    print(f"\n[*] Reading reference tree at PS2 0x{ref_offset:X}...")
    ref_data = read_remote_mem(hProcess, eemem_base + ref_offset, ref_size + 12)
    if ref_data:
        ref_root, _ = parse_node(ref_data, 0)
        print("\n==========================================")
        print("  REFERENCE MODEL TREE IN RAM (CORRECT PARSER)")
        print("==========================================")
        dump_tree(ref_root)
        
    print(f"\n[*] Reading patched tree at PS2 0x{pat_offset:X}...")
    pat_data = read_remote_mem(hProcess, eemem_base + pat_offset, pat_size + 12)
    if pat_data:
        pat_root, _ = parse_node(pat_data, 0)
        print("\n==========================================")
        print("  PATCHED MODEL TREE IN RAM (CORRECT PARSER)")
        print("==========================================")
        dump_tree(pat_root)

if __name__ == '__main__':
    main()
