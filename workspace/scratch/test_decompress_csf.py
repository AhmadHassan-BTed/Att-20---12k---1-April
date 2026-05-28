import os
import zlib

def decompress_csf():
    filepath = "workspace/CHARSEL1.CSF"
    with open(filepath, 'rb') as f:
        data = f.read()
        
    if data.startswith(b'CESF'):
        # Try finding 78 DA
        idx = data.find(b'\x78\xda')
        if idx != -1:
            print(f"Found zlib header at offset 0x{idx:02X}")
            try:
                decompressed = zlib.decompress(data[idx:])
                print(f"Decompressed size: {len(decompressed)} bytes")
                print(f"First 16 bytes: {decompressed[:16]}")
            except Exception as e:
                print(f"Zlib decompression failed: {e}")
        else:
            print("No zlib header found")

if __name__ == "__main__":
    decompress_csf()
