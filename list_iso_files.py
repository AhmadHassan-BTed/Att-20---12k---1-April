import pycdlib

def list_iso(iso_path):
    print(f"\n=== LISTING FILES IN: {iso_path} ===")
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
        for root, dirs, files in iso.walk(iso_path='/'):
            for f in files:
                fpath = f"{root}/{f}"
                try:
                    # Some files might have double semi-colons or special formats, pycdlib walk handles it
                    print(f"  {fpath}")
                except Exception as e:
                    print(f"  {fpath} (Error printing: {e})")
        iso.close()
    except Exception as e:
        print(f"[-] Error: {e}")

list_iso('EQOA_Original.iso')
list_iso('EQOA_Frontiers.iso')
