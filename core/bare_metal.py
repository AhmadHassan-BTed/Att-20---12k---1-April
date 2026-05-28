import os
import shutil
import pycdlib
import sys

def extract_iso(iso_path, out_dir):
    print(f"[*] Extracting {iso_path} to {out_dir}...")
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir)
    
    iso = pycdlib.PyCdlib()
    iso.open(iso_path)
    
    for dirname, dirnames, filenames in iso.walk(iso_path='/'):
        # Create directories
        for d in dirnames:
            out_d = os.path.join(out_dir, (dirname + ('/' if dirname != '/' else '') + d).strip('/').replace('/', os.sep))
            os.makedirs(out_d, exist_ok=True)
            
        # Extract files
        for f in filenames:
            iso_file_path = dirname + ('/' if dirname != '/' else '') + f
            # Remove ;1 from ISO filenames
            out_name = f.split(';')[0] if ';' in f else f
            out_f = os.path.join(out_dir, dirname.strip('/').replace('/', os.sep), out_name)
            
            # Write out
            with open(out_f, 'wb') as out_file:
                iso.get_file_from_iso_fp(out_file, iso_path=iso_file_path)
                
    iso.close()
    print("[+] Extraction complete.")

def main():
    unmod_iso = "iso/unmodified/EQOA_Frontiers.iso"
    extracted_dir = "workspace/ISO_EXTRACTED"
    patched_esf = "workspace/FINAL_CHAR_MERGED.ESF"
    
    # Phase 1: Absolute Deconstruction
    extract_iso(unmod_iso, extracted_dir)
    
    target_char_esf = None
    for root, dirs, files in os.walk(extracted_dir):
        for file in files:
            if file == "CHAR.ESF":
                target_char_esf = os.path.join(root, file)
                break
        if target_char_esf:
            break
            
    if not target_char_esf:
        print("[-] FATAL: CHAR.ESF not found in extracted ISO.")
        sys.exit(1)
        
    orig_size = os.path.getsize(target_char_esf)
    print(f"\n[*] PHASE 1: Original File Found")
    print(f"    Path: {target_char_esf}")
    print(f"    Original Size: {orig_size:,} bytes")
    
    # Phase 2: The Physical Override
    print(f"\n[*] PHASE 2: Physical Override")
    shutil.copy2(patched_esf, target_char_esf)
    new_size = os.path.getsize(target_char_esf)
    print(f"    Path: {target_char_esf}")
    print(f"    New Size: {new_size:,} bytes")
    
    if orig_size == new_size:
        print("[-] OVERRIDE FAILED: File sizes match. The copy did not apply.")
    else:
        print("[+] OVERRIDE SUCCESS: File size changed.")

if __name__ == "__main__":
    main()
