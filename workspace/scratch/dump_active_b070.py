import os
import sys
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

def find_pcsx2_pid():
    target_names = [b"pcsx2.exe", b"pcsx2-qt.exe", b"pcsx2x64.exe"]
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    pe = PROCESSENTRY32()
    pe.dwSize = ctypes.sizeof(PROCESSENTRY32)
    found_pid, found_name = None, None
    if kernel32.Process32First(snapshot, ctypes.byref(pe)):
        while True:
            for t in target_names:
                if t in pe.szExeFile.lower():
                    found_pid = pe.th32ProcessID
                    found_name = pe.szExeFile.decode('utf-8', errors='replace')
                    break
            if found_pid or not kernel32.Process32Next(snapshot, ctypes.byref(pe)):
                break
    kernel32.CloseHandle(snapshot)
    return found_pid, found_name

def get_module_base(pid, exe_name):
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)
    me = MODULEENTRY32()
    me.dwSize = ctypes.sizeof(MODULEENTRY32)
    base_addr = None
    if kernel32.Module32First(snapshot, ctypes.byref(me)):
        while True:
            if exe_name.lower().encode() in me.szModule.lower():
                base_addr = me.modBaseAddr
                break
            if not kernel32.Module32Next(snapshot, ctypes.byref(me)):
                break
    kernel32.CloseHandle(snapshot)
    return base_addr

def read_remote_mem(proc_handle, address, size):
    buf = ctypes.create_string_buffer(size)
    bytes_read = ctypes.c_size_t(0)
    ok = kernel32.ReadProcessMemory(proc_handle, ctypes.c_void_p(address), buf, size, ctypes.byref(bytes_read))
    return buf.raw[:bytes_read.value] if ok else None

def find_eemem_base(proc_handle, pid, exe_name):
    base_addr = get_module_base(pid, exe_name)
    dos_hdr = read_remote_mem(proc_handle, base_addr, 64)
    pe_offset = struct.unpack_from('<I', dos_hdr, 0x3c)[0]
    pe_hdr = read_remote_mem(proc_handle, base_addr + pe_offset, 264)
    magic = struct.unpack_from('<H', pe_hdr, 24)[0]
    is_64 = (magic == 0x20B)
    export_dir_offset = 24 + 112 if is_64 else 24 + 96
    export_rva, _ = struct.unpack_from('<II', pe_hdr, export_dir_offset)
    export_dir_bytes = read_remote_mem(proc_handle, base_addr + export_rva, 40)
    num_names = struct.unpack_from('<I', export_dir_bytes, 24)[0]
    funcs_rva = struct.unpack_from('<I', export_dir_bytes, 28)[0]
    names_rva = struct.unpack_from('<I', export_dir_bytes, 32)[0]
    ords_rva = struct.unpack_from('<I', export_dir_bytes, 36)[0]
    names_data = read_remote_mem(proc_handle, base_addr + names_rva, num_names * 4)
    ords_data = read_remote_mem(proc_handle, base_addr + ords_rva, num_names * 2)
    funcs_data = read_remote_mem(proc_handle, base_addr + funcs_rva, num_names * 4)
    for i in range(num_names):
        name_rva = struct.unpack_from('<I', names_data, i * 4)[0]
        name_bytes = read_remote_mem(proc_handle, base_addr + name_rva, 64)
        if name_bytes and name_bytes.split(b'\x00')[0].decode() == "EEmem":
            ord_val = struct.unpack_from('<H', ords_data, i * 2)[0]
            func_rva = struct.unpack_from('<I', funcs_data, ord_val * 4)[0]
            eemem_ptr = base_addr + func_rva
            ptr_bytes = read_remote_mem(proc_handle, eemem_ptr, 8)
            return struct.unpack('<Q', ptr_bytes)[0]
    return None

def format_hex(data, start_offset):
    lines = []
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_str = " ".join(f"{b:02X}" for b in chunk)
        ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
        lines.append(f"  {start_offset+i:08X}: {hex_str:<47} |{ascii_str}|")
    return "\n".join(lines)

def main():
    pid, name = find_pcsx2_pid()
    hProcess = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid) or kernel32.OpenProcess(PROCESS_VM_READ, False, pid)
    eemem = find_eemem_base(hProcess, pid, name)
    
    # Patched character root is at 0x00F9DD37
    # 0x0B070 is Child 3. Let's parse root to get its exact children offsets.
    root_raw = read_remote_mem(hProcess, eemem + 0x00F9DD37, 505943)
    
    # Custom simple parse to find 0x0B070 absolute position
    pos = 12
    # Child 0
    c0_type = struct.unpack_from('<I', root_raw, pos)[0]
    c0_size = struct.unpack_from('<I', root_raw, pos + 4)[0]
    print(f"Child 0: type=0x{c0_type:X}, size={c0_size}")
    pos += 12 + c0_size
    
    # Child 1
    c1_type = struct.unpack_from('<I', root_raw, pos)[0]
    c1_size = struct.unpack_from('<I', root_raw, pos + 4)[0]
    print(f"Child 1: type=0x{c1_type:X}, size={c1_size}")
    pos += 12 + c1_size
    
    # Child 2
    c2_type = struct.unpack_from('<I', root_raw, pos)[0]
    c2_size = struct.unpack_from('<I', root_raw, pos + 4)[0]
    print(f"Child 2: type=0x{c2_type:X}, size={c2_size}")
    pos += 12 + c2_size
    
    # Child 3 (0x0B070)
    c3_pos = pos
    c3_type = struct.unpack_from('<I', root_raw, pos)[0]
    c3_size = struct.unpack_from('<I', root_raw, pos + 4)[0]
    print(f"Child 3: type=0x{c3_type:X}, size={c3_size} (offset relative to root: 0x{pos:X})")
    
    # Now let's dump bytes from Child 3 offset + 12 + 9950 to + 17050
    # Let's inspect the transition around Child 3's Child 3 (starts at 9964 relative to 0x0B070)
    # Child 3 size is 6964. So it ends at 9964 + 12 + 6964 = 16940.
    start_dump_rel = 9976 + 6900
    dump_bytes = root_raw[c3_pos + 12 + start_dump_rel : c3_pos + 12 + start_dump_rel + 200]
    
    print("\n=== RAW HEX DUMP OF TRANSITION IN ACTIVE RAM ===")
    print(format_hex(dump_bytes, 0x00F9DD37 + c3_pos + 12 + start_dump_rel))
    
    kernel32.CloseHandle(hProcess)

if __name__ == '__main__':
    main()
