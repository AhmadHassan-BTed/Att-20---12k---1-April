import os, zlib, struct
import pycdlib
from esf_parser import ESFParser

# 1. Extract CHARSEL1.CSF from EQOA_Frontiers.iso
iso = pycdlib.PyCdlib()
iso.open('EQOA_Frontiers.iso')
with open('workspace/CHARSEL1.CSF', 'wb') as out_f:
    iso.get_file_from_iso_fp(out_f, iso_path='/DATA2/CHARSEL1.CSF;1')
iso.close()
print("[+] Extracted CHARSEL1.CSF successfully!")

# 2. Decompress CSF -> ESF
# CSF format has CESF magic, followed by zlib blocks.
# Let's write a robust parser to decompress it!
def decompress_csf(csf_path, esf_out_path):
    with open(csf_path, 'rb') as f:
        data = f.read()
    
    if data[:4] != b'CESF':
        print("[-] Error: Invalid CSF magic!")
        return False
        
    print("[*] Decompressing CSF blocks...")
    out_data = bytearray()
    
    # After 'CESF' magic (4 bytes), what follows?
    # Let's inspect bytes 4-12. Usually it's some header.
    # Let's see: zlib compressed streams start with 0x78 0x9C or similar.
    # Let's find zlib signatures or read block lengths.
    # Let's print the first 64 bytes of the CSF file.
    print("CSF Hex Header: ", data[:64].hex())
    
    # According to Report.md:
    # "zlib blocks of 256 KB each. Inter-block headers: 8 bytes between each compressed block"
    # Let's see: the first 8 bytes of the file might be 'CESF' (4 bytes) + uncompressed size (4 bytes)?
    # Let's read the block sizes or just search for zlib headers (0x78 0x9C).
    pos = 8
    while pos < len(data):
        # Check if we have a zlib header
        if pos + 2 <= len(data) and data[pos] == 0x78 and data[pos+1] == 0x9C:
            # We found a zlib block!
            # Let's decompress it using zlib.decompressobj
            dobj = zlib.decompressobj()
            try:
                decompressed = dobj.decompress(data[pos:])
                out_data.extend(decompressed)
                # Advance pos by the number of compressed bytes consumed
                consumed = len(data[pos:]) - len(dobj.unused_data)
                print(f"  Decompressed zlib block at offset 0x{pos:X}: consumed {consumed} bytes -> {len(decompressed)} bytes")
                pos += consumed
                
                # Check for 8-byte inter-block header
                if pos < len(data):
                    inter_header = data[pos:pos+8]
                    print(f"  Inter-block header at 0x{pos:X}: {inter_header.hex()}")
                    pos += 8
            except Exception as e:
                print(f"[-] Decompression error at offset 0x{pos:X}: {e}")
                break
        else:
            # Advance byte-by-byte or look for next 0x78 0x9C
            pos += 1
            
    if len(out_data) > 0:
        with open(esf_out_path, 'wb') as out_f:
            out_f.write(out_data)
        print(f"[+] Successfully decompressed to '{esf_out_path}' ({len(out_data):,} bytes)")
        return True
    return False

decompress_csf('workspace/CHARSEL1.CSF', 'workspace/CHARSEL1.ESF')
