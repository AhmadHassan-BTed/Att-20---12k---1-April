import os
import sys
import hashlib
import traceback
import pycdlib

def build_iso():
    out_iso = "MANUAL_PATCH.iso"
    src_dir = "workspace/ISO_EXTRACTED"
    
    try:
        iso = pycdlib.PyCdlib()
        iso.new(interchange_level=3)
        
        for root, dirs, files in os.walk(src_dir):
            rel_path = os.path.relpath(root, src_dir)
            
            # Format path for ISO 9660 (uppercase)
            if rel_path == '.':
                iso_base = ''
            else:
                iso_base = '/' + rel_path.replace('\\', '/').upper()
            
            for d in dirs:
                iso_dir = iso_base + '/' + d.upper()
                iso.add_directory(iso_path=iso_dir)
                
            for f in files:
                file_path = os.path.join(root, f)
                iso_path = iso_base + '/' + f.upper() + ';1'
                iso.add_file(file_path, iso_path=iso_path)
                
        print("[*] Writing ISO out to disk...")
        iso.write(out_iso)
        iso.close()
        
        print("[+] Build Complete. Verifying hash...")
        sha256 = hashlib.sha256()
        with open(out_iso, 'rb') as f:
            for chunk in iter(lambda: f.read(4 * 1024 * 1024), b''):
                sha256.update(chunk)
                
        print(f"Hash: {sha256.hexdigest()}")
        print(f"Location: {os.path.abspath(out_iso)}")
        
    except Exception as e:
        print(f"[-] FATAL ERROR DURING BUILD: {e}")
        print("[*] Falling back to standard mkisofs...")
        ret = os.system('mkisofs -o MANUAL_PATCH.iso -iso-level 2 -V "EQOA_FRONTIERS" -joliet-long workspace/ISO_EXTRACTED')
        if ret == 0 and os.path.exists(out_iso):
            print(f"[+] mkisofs Build Complete. Location: {os.path.abspath(out_iso)}")
        else:
            print("[-] mkisofs also failed.")
            sys.exit(1)

if __name__ == '__main__':
    build_iso()
