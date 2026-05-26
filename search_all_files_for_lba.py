import struct, pycdlib, os

iso = pycdlib.PyCdlib()
iso.open('EQOA_Frontiers.iso')

files_to_check = [
    ('/UTIL.REL;1', 'workspace/UTIL.REL'),
    ('/CLIENT.AUT;1', 'workspace/CLIENT.AUT'),
    ('/STATION.AUT;1', 'workspace/STATION.AUT'),
    ('/STATION.PAK;1', 'workspace/STATION.PAK'),
    ('/SUPPORT.AUT;1', 'workspace/SUPPORT.AUT'),
]

target_lba = 3578
target_size = 148370972

for internal_path, local_path in files_to_check:
    print(f"\n[*] Extracting and checking {internal_path}...")
    try:
        with open(local_path, 'wb') as out_f:
            iso.get_file_from_iso_fp(out_f, iso_path=internal_path)
            
        with open(local_path, 'rb') as f:
            data = f.read()
            
        lba_matches = []
        size_matches = []
        for i in range(0, len(data) - 4, 4):
            val = struct.unpack_from('<I', data, i)[0]
            if val == target_lba:
                lba_matches.append(i)
            if val == target_size:
                size_matches.append(i)
                
        if lba_matches:
            print(f"  [+] Found Target LBA 3578 at byte offsets: {lba_matches}")
        if size_matches:
            print(f"  [+] Found Target Size 148370972 at byte offsets: {size_matches}")
            
    except Exception as e:
        print(f"  [-] Error: {e}")

iso.close()
