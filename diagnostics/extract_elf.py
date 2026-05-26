import pycdlib

iso = pycdlib.PyCdlib()
iso.open('EQOA_Frontiers.iso')
with open('workspace/SLUS_207.44', 'wb') as out_f:
    iso.get_file_from_iso_fp(out_f, iso_path='/SLUS_207.44;1')
iso.close()
print("[+] Extracted SLUS_207.44 successfully!")
