import os
from eqoa_toolkit import CSFFile, ESFFile

# Extract and decompress CHARCUST.CSF from both ISOs
for label, iso_name in [('Original', 'EQOA_Original.iso'), ('Frontiers', 'EQOA_Frontiers.iso')]:
    csf_path = f"workspace/CHARCUST_{label}.CSF"
    esf_path = f"workspace/CHARCUST_{label}.ESF"
    
    # Extract from ISO
    import pycdlib
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_name)
        with open(csf_path, 'wb') as out_f:
            iso.get_file_from_iso_fp(out_f, iso_path='/DATA2/CHARCUST.CSF;1')
        iso.close()
    except Exception as e:
        if iso: iso.close()
        print(f"[-] Could not extract {label} CHARCUST.CSF: {e}")
        continue
        
    # Decompress
    try:
        csf = CSFFile(csf_path)
        esf_bytes = csf.decompress()
        with open(esf_path, 'wb') as f:
            f.write(esf_bytes)
        
        esf = ESFFile(esf_path)
        print(f"\n=== {label} CHARCUST.ESF ===")
        print(f"  Version: {esf.version}")
        print(f"  Model Count: {esf.get_model_count()}")
        
        # Check first 5 children under model container
        container = esf.get_model_container()
        if container:
            print(f"  Container Type: 0x{container.type_id:05X}, children count: {container.child_count}")
            for idx, child in enumerate(container.children[:5]):
                c_hash = child.get_hash()
                hash_str = f"0x{c_hash:08X}" if c_hash else "None"
                print(f"    [{idx}]: type=0x{child.type_id:05X}, size={child.total_size:,} bytes, hash={hash_str}")
            if len(container.children) > 5:
                print(f"    ... and {len(container.children) - 5} more children")
    except Exception as e:
        print(f"[-] Error processing {label} CHARCUST: {e}")
