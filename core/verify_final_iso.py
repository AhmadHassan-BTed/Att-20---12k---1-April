import sys
import os
import pycdlib
import hashlib
import io

def get_iso_file_info(iso_path, target_filename):
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
        target_path = None
        for dirname, dirnames, filenames in iso.walk(iso_path='/'):
            for fn in filenames:
                if target_filename in fn:
                    target_path = dirname + ('/' if dirname != '/' else '') + fn
                    break
            if target_path: break
            
        if not target_path:
            iso.close()
            return None, None
            
        h = hashlib.sha256()
        header = None
        with iso.open_file_from_iso(iso_path=target_path) as f:
            # Read first 64 bytes for header
            header = f.read(64)
            h.update(header)
            for chunk in iter(lambda: f.read(65536), b''):
                h.update(chunk)
                
        iso.close()
        return h.hexdigest(), header
    except Exception as e:
        return f"Error: {e}", None

def main():
    unmod_iso = "iso/unpatched/EQOA_Frontiers.iso"
    patch_iso = "iso/patched/EQOA_Frontiers_Patched.iso"
    
    if not os.path.exists(unmod_iso) or not os.path.exists(patch_iso):
        print("[-] ISO files not found.")
        sys.exit(1)
        
    print("[*] Commencing Bitwise Compare of CHAR.ESF inside ISOs...")
    
    unmod_hash, _ = get_iso_file_info(unmod_iso, 'CHAR.ESF')
    patch_hash, patch_header = get_iso_file_info(patch_iso, 'CHAR.ESF')
    
    print(f"    Original CHAR.ESF SHA256: {unmod_hash}")
    print(f"    Patched  CHAR.ESF SHA256: {patch_hash}")
    
    if patch_header:
        hex_sig = " ".join([f"{b:02X}" for b in patch_header[:32]])
        print(f"\n[+] FINAL PATCHED HEADER HEX-SIGNATURE (First 32 bytes):")
        print(f"    {hex_sig}")
        
    if unmod_hash == patch_hash:
        print("\n[FAILURE] The hashes are IDENTICAL. The rebuilder failed to overwrite the payload!")
        sys.exit(1)
    else:
        print("\n[SUCCESS] The hashes differ. The CHAR.ESF file was successfully overwritten in the ISO.")

if __name__ == "__main__":
    main()
