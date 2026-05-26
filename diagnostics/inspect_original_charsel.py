import os
from eqoa_toolkit import CSFFile, ESFFile

# Extract and decompress CHARSEL1 to CHARSEL4 from Original ISO
for charsel_idx in range(1, 5):
    filename = f"CHARSEL{charsel_idx}"
    csf_path = f"workspace/{filename}_orig.CSF"
    esf_path = f"workspace/{filename}_orig.ESF"
    
    # Extract from Original ISO
    import pycdlib
    iso = pycdlib.PyCdlib()
    try:
        iso.open('EQOA_Original.iso')
        with open(csf_path, 'wb') as out_f:
            iso.get_file_from_iso_fp(out_f, iso_path=f'/DATA2/{filename}.CSF;1')
        iso.close()
    except Exception as e:
        if iso: iso.close()
        print(f"[-] Could not extract original {filename}.CSF: {e}")
        continue
        
    # Decompress
    try:
        csf = CSFFile(csf_path)
        esf_bytes = csf.decompress()
        with open(esf_path, 'wb') as f:
            f.write(esf_bytes)
        
        esf = ESFFile(esf_path)
        print(f"\n=== Original {filename}.ESF (models count = {esf.get_model_count()}) ===")
        models = esf.get_models()
        for idx, m in enumerate(models[:10]):
            m_hash = m.get_hash()
            hash_str = f"0x{m_hash:08X}" if m_hash else "None"
            print(f"  [{idx}]: type=0x{m.type_id:05X}, size={m.total_size:,} bytes, hash={hash_str}")
    except Exception as e:
        print(f"[-] Error processing original {filename}: {e}")
