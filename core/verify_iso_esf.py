import sys
import os
import pycdlib
import io

def get_esf_header(iso_path):
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
        
        target_path = None
        for dirname, dirnames, filenames in iso.walk(iso_path='/'):
            for filename in filenames:
                if 'CHAR.ESF' in filename:
                    target_path = dirname + ('/' if dirname != '/' else '') + filename
                    break
            if target_path:
                break
                
        if not target_path:
            iso.close()
            return b"Error: CHAR.ESF not found in ISO"
            
        extracted = io.BytesIO()
        iso.get_file_from_iso_fp(extracted, iso_path=target_path)
        
        data = extracted.getvalue()
        iso.close()
        return data[:32]
    except Exception as e:
        iso.close()
        return f"Error: {e}".encode()

def main():
    unmod_iso = "iso/unpatched/EQOA_Frontiers.iso"
    patch_iso = "iso/patched/EQOA_Frontiers_Patched.iso"
    
    print("\nExecuting Step 2: The ESF Deep Dive...")
    
    unmod_header = get_esf_header(unmod_iso)
    patch_header = get_esf_header(patch_iso)
    
    def format_hex(data):
        if isinstance(data, bytes):
            return " ".join([f"{b:02X}" for b in data])
        return data
        
    print(f"Source CHAR.ESF Header:  {format_hex(unmod_header)}")
    print(f"Patched CHAR.ESF Header: {format_hex(patch_header)}")
    
    if unmod_header == patch_header:
        print("\n[FAILURE] The first 32 bytes of CHAR.ESF inside the ISOs are IDENTICAL.")
        print("This means the rebuilder is failing to overwrite the ESF data in the patched ISO, or the header is not supposed to change.")
    else:
        print("\n[SUCCESS] The headers are different.")

if __name__ == "__main__":
    main()
