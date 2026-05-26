import pycdlib

def check_layout(iso_path):
    print(f"\n=== SECTOR LAYOUT OF: {iso_path} ===")
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
        files = []
        for root, dirs, filenames in iso.walk(iso_path='/'):
            for f in filenames:
                fpath = f"{root}/{f}"
                try:
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
                    
                    if extent is not None:
                        files.append((extent, record.data_length, fpath))
                except Exception as e:
                    pass
        
        # Sort files by their starting sector LBA
        files.sort(key=lambda x: x[0])
        
        for extent, length, path in files:
            sectors = length // 2048
            if length % 2048 != 0:
                sectors += 1
            print(f"  LBA: {extent:7d} | Sectors: {sectors:5d} | Size: {length:11,} B | {path}")
            
        iso.close()
    except Exception as e:
        print(f"[-] Error: {e}")

check_layout('EQOA_Frontiers.iso')
