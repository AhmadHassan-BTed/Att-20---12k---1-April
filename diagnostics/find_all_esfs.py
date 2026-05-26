import pycdlib

def list_iso_esfs(iso_path):
    print(f"\n=== ESF/CSF Files in: {iso_path} ===")
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
        for root, dirs, files in iso.walk(iso_path='/'):
            for f in files:
                if '.ESF' in f.upper() or '.CSF' in f.upper():
                    fpath = f"{root}/{f}"
                    record = iso.get_record(iso_path=fpath)
                    print(f"  {fpath:<30} | Size: {record.data_length:,} bytes")
        iso.close()
    except Exception as e:
        print(f"[-] Error: {e}")

list_iso_esfs('EQOA_Original.iso')
list_iso_esfs('EQOA_Frontiers.iso')
