import pycdlib

def find_char_esf(iso_path):
    iso = pycdlib.PyCdlib()
    iso.open(iso_path)
    
    # Walk directory to find /DATA/CHAR.ESF;1
    for root, dirs, files in iso.walk(iso_path='/'):
        for f in files:
            if 'CHAR.ESF' in f.upper():
                fpath = f"{root}/{f}"
                record = iso.get_record(iso_path=fpath)
                extent = None
                for attr in ['extent_loc', 'extent_location', 'location', 'extent']:
                    if hasattr(record, attr):
                        val = getattr(record, attr)
                        if callable(val):
                            extent = val()
                        else:
                            extent = val
                        break
                print(f"File: {fpath}")
                print(f"  LBA: {extent}")
                print(f"  Size: {record.data_length} bytes")
                print(f"  Sectors: {record.data_length // 2048 + (1 if record.data_length % 2048 != 0 else 0)}")
                
    iso.close()

print("=== frontiers iso ===")
find_char_esf('EQOA_Frontiers.iso')
