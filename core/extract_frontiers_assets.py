import os
import sys
import pycdlib

def main():
    print("=" * 80)
    print("  EQOA FRONTIERS BASELINE ASSETS EXTRACTOR")
    print("=" * 80)
    
    iso_path = 'iso/unpatched/EQOA_Frontiers.iso'
    output_dir = 'assets/Frontiers'
    
    if not os.path.exists(iso_path):
        print(f"[-] Error: Could not find clean baseline ISO at {iso_path}")
        print("[-] Please run setup_environment.bat or place the file manually.")
        sys.exit(1)
        
    os.makedirs(os.path.join(output_dir, 'data'), exist_ok=True)
    os.makedirs(os.path.join(output_dir, 'data2'), exist_ok=True)
    
    files_to_extract = {
        '/DATA/CHAR.ESF;1': 'data/CHAR.ESF',
        '/DATA2/CHARCUST.CSF;1': 'data2/CHARCUST.CSF',
        '/DATA2/CHARFACE.CSF;1': 'data2/CHARFACE.CSF',
        '/DATA2/CHARFACE.ESF;1': 'data2/CHARFACE.ESF',
        '/DATA2/CHARSEL1.CSF;1': 'data2/CHARSEL1.CSF',
        '/DATA2/CHARSEL2.CSF;1': 'data2/CHARSEL2.CSF',
        '/DATA2/CHARSEL3.CSF;1': 'data2/CHARSEL3.CSF',
        '/DATA2/CHARSEL4.CSF;1': 'data2/CHARSEL4.CSF',
    }
    
    iso = pycdlib.PyCdlib()
    try:
        print(f"[*] Opening {iso_path}...")
        iso.open(iso_path)
        
        for iso_path_in, rel_out_path in files_to_extract.items():
            dest = os.path.join(output_dir, rel_out_path)
            print(f"  [*] Extracting {iso_path_in} -> {dest} ...")
            with open(dest, 'wb') as out_f:
                iso.get_file_from_iso_fp(out_f, iso_path=iso_path_in)
            print(f"    [+] Extracted successfully ({os.path.getsize(dest):,} bytes)")
            
        iso.close()
        print("\n[+] Extraction Complete! Baseline Frontiers assets successfully saved to 'assets/Frontiers/'.")
    except Exception as e:
        if iso:
            iso.close()
        print(f"[-] Error during extraction: {e}")
        sys.exit(1)
        
    print("=" * 80)

if __name__ == '__main__':
    main()
