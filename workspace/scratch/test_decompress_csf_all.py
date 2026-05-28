import os
import zlib

def decompress_csf():
    filepath = "workspace/CHARSEL1.CSF"
    with open(filepath, 'rb') as f:
        data = f.read()
        
    out_buf = bytearray()
    pos = 0
    while pos < len(data):
        idx = data.find(b'\x78\xda', pos)
        if idx == -1:
            break
            
        try:
            d = zlib.decompressobj()
            chunk = d.decompress(data[idx:])
            out_buf.extend(chunk)
            # Advance past this stream
            # unused_data starts where the stream ended
            consumed = len(data[idx:]) - len(d.unused_data)
            pos = idx + consumed
            print(f"Stream at 0x{idx:02X} decompressed {len(chunk)} bytes. Consumed {consumed} bytes.")
        except Exception as e:
            print(f"Failed at 0x{idx:02X}: {e}")
            pos = idx + 2
            
    print(f"Total decompressed size: {len(out_buf)}")
    
    with open("workspace/scratch/CHARSEL1_DECOMPRESSED.ESF", 'wb') as f:
        f.write(out_buf)

if __name__ == "__main__":
    decompress_csf()
