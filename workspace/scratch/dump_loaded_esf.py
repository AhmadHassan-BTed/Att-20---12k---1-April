import sys
import struct
import ctypes

sys.path.append(r't:\Att 20 - 12k - 1 April')

PROCESS_ALL_ACCESS = 0x001F0FFF
PROCESS_VM_READ = 0x0010
PROCESS_QUERY_INFORMATION = 0x0400
TH32CS_SNAPPROCESS = 0x00000002
TH32CS_SNAPMODULE = 0x00000008
TH32CS_SNAPMODULE32 = 0x00000010

kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
from ctypes.wintypes import DWORD, HMODULE

class PROCESSENTRY32(ctypes.Structure):
    _fields_ = [
        ("dwSize", DWORD),
        ("cntUsage", DWORD),
        ("th32ProcessID", DWORD),
        ("th32DefaultHeapID", ctypes.POINTER(ctypes.c_ulong)),
        ("th32ModuleID", DWORD),
        ("cntThreads", DWORD),
        ("th32ParentProcessID", DWORD),
        ("pcPriClassBase", ctypes.c_long),
        ("dwFlags", DWORD),
        ("szExeFile", ctypes.c_char * 260),
    ]

class MODULEENTRY32(ctypes.Structure):
    _fields_ = [
        ("dwSize", DWORD),
        ("th32ModuleID", DWORD),
        ("th32ProcessID", DWORD),
        ("GlblcntUsage", DWORD),
        ("ProccntUsage", DWORD),
        ("modBaseAddr", ctypes.c_void_p),
        ("modBaseSize", DWORD),
        ("hModule", HMODULE),
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
    hProcess = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    eemem = find_eemem_base(hProcess, pid, name)
    
    # CHAR.ESF in memory is at 0x0186CA40
    data = read_remote_mem(hProcess, eemem + 0x0186CA40, 128)
    print("=== DUMP OF 0x0186CA40 IN RAM ===")
    print(format_hex(data, 0x0186CA40))
    
    kernel32.CloseHandle(hProcess)

if __name__ == '__main__':
    main()
