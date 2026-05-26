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

# Log accumulator
log_lines = []
def log(msg):
    print(msg)
    log_lines.append(msg)

def find_pcsx2_pid():
    target_names = [b"pcsx2.exe", b"pcsx2-qt.exe", b"pcsx2x64.exe",
                    b"pcsx2-qtx64.exe", b"pcsx2-avx2.exe"]
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    if snapshot == -1 or snapshot == ctypes.c_void_p(-1).value:
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
            if found_pid or not kernel32.Process32Next(snapshot, ctypes.byref(pe)):
                break
    kernel32.CloseHandle(snapshot)
    return found_pid, found_name

def get_module_base(pid, module_name):
    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)
    if snapshot == -1 or snapshot == ctypes.c_void_p(-1).value:
        log(f"[-] Failed to get modules snapshot: {ctypes.get_last_error()}")
        return None
    me = MODULEENTRY32()
    me.dwSize = ctypes.sizeof(MODULEENTRY32)
    base_addr = None
    if kernel32.Module32First(snapshot, ctypes.byref(me)):
        while True:
            name = me.szModule.lower()
            if module_name.lower().encode() in name:
                base_addr = me.modBaseAddr
                break
            if not kernel32.Module32Next(snapshot, ctypes.byref(me)):
                break
    kernel32.CloseHandle(snapshot)
    return base_addr

def read_mem(hProcess, address, size):
    buf = ctypes.create_string_buffer(size)
    bytes_read = ctypes.c_size_t(0)
    ok = kernel32.ReadProcessMemory(hProcess, ctypes.c_void_p(address), buf, size, ctypes.byref(bytes_read))
    if not ok:
        return None
    return buf.raw[:bytes_read.value]

def main():
    try:
        pid, name = find_pcsx2_pid()
        if not pid:
            log("[-] PCSX2 not found.")
            return
        log(f"[+] Found process: {name} (PID: {pid})")
        
        hProcess = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
        if not hProcess:
            hProcess = kernel32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, pid)
        if not hProcess:
            log(f"[-] OpenProcess failed: {ctypes.get_last_error()}")
            return
            
        log(f"[+] Process opened: {hProcess:X}")
        base_addr = get_module_base(pid, name)
        if not base_addr:
            log("[-] Module base not found.")
            return
        log(f"[+] Module base address: 0x{base_addr:X}")
        
        # Read PE headers
        dos_header = read_mem(hProcess, base_addr, 64)
        if not dos_header or dos_header[:2] != b'MZ':
            log("[-] Invalid DOS header.")
            return
        
        pe_offset = struct.unpack_from('<I', dos_header, 0x3C)[0]
        pe_header = read_mem(hProcess, base_addr + pe_offset, 264)
        if not pe_header or pe_header[:4] != b'PE\x00\x00':
            log("[-] Invalid PE header.")
            return
        
        # Check Magic to see if 64-bit
        magic = struct.unpack_from('<H', pe_header, 24)[0]
        is_64 = (magic == 0x20B)
        log(f"[+] PE Architecture: {'64-bit' if is_64 else '32-bit'}")
        
        # Optional Header Data Directories
        if is_64:
            export_dir_offset = 24 + 112  # offset of Export Directory in optional header for PE32+
        else:
            export_dir_offset = 24 + 96   # for PE32
            
        export_rva, export_size = struct.unpack_from('<II', pe_header, export_dir_offset)
        if export_rva == 0:
            log("[-] No export table found in PE.")
            return
            
        log(f"[+] Export Table RVA: 0x{export_rva:X}, Size: {export_size}")
        
        # Read IMAGE_EXPORT_DIRECTORY
        export_bytes = read_mem(hProcess, base_addr + export_rva, 40)
        if not export_bytes:
            log("[-] Failed to read export directory.")
            return
            
        num_funcs = struct.unpack_from('<I', export_bytes, 20)[0]
        num_names = struct.unpack_from('<I', export_bytes, 24)[0]
        funcs_rva = struct.unpack_from('<I', export_bytes, 28)[0]
        names_rva = struct.unpack_from('<I', export_bytes, 32)[0]
        ords_rva = struct.unpack_from('<I', export_bytes, 36)[0]
        
        log(f"[+] Exports count: {num_funcs} functions, {num_names} named")
        
        # Read name pointer table and ordinals
        names_data = read_mem(hProcess, base_addr + names_rva, num_names * 4)
        ords_data = read_mem(hProcess, base_addr + ords_rva, num_names * 2)
        funcs_data = read_mem(hProcess, base_addr + funcs_rva, num_funcs * 4)
        
        if not names_data or not ords_data or not funcs_data:
            log("[-] Failed to read export tables.")
            return
            
        # Search for EEmem
        eemem_ptr = None
        for i in range(num_names):
            name_rva = struct.unpack_from('<I', names_data, i * 4)[0]
            # Read name string (max 64 bytes for safety)
            name_str_bytes = read_mem(hProcess, base_addr + name_rva, 64)
            if not name_str_bytes:
                continue
            name_str = name_str_bytes.split(b'\x00')[0].decode('utf-8', errors='replace')
            if name_str == "EEmem":
                ord_val = struct.unpack_from('<H', ords_data, i * 2)[0]
                func_rva = struct.unpack_from('<I', funcs_data, ord_val * 4)[0]
                eemem_ptr = base_addr + func_rva
                log(f"[+] Found 'EEmem' export at RVA 0x{func_rva:X} (Address: 0x{eemem_ptr:X})")
                break
                
        if not eemem_ptr:
            log("[-] 'EEmem' export not found.")
            return
            
        # Read the 8-byte pointer value stored at eemem_ptr
        ptr_bytes = read_mem(hProcess, eemem_ptr, 8)
        if not ptr_bytes:
            log("[-] Failed to read memory at EEmem pointer address.")
            return
            
        eemem_base = struct.unpack('<Q', ptr_bytes)[0]
        log(f"[+] Actual EE RAM Base Address (dereferenced EEmem): 0x{eemem_base:X}")
        
        # Read first 16 bytes of EE RAM to check what is in there
        first_bytes = read_mem(hProcess, eemem_base, 16)
        if first_bytes:
            log(f"[+] First 16 bytes of EE RAM: {first_bytes.hex()}")
        else:
            log("[-] Failed to read first 16 bytes of EE RAM.")
            
    finally:
        # Write log to file
        os.makedirs('diagnostics', exist_ok=True)
        with open('diagnostics/test_eemem_results.txt', 'w') as f:
            f.write('\n'.join(log_lines) + '\n')
        print("[*] Diagnostic results written to diagnostics/test_eemem_results.txt")

if __name__ == '__main__':
    main()
