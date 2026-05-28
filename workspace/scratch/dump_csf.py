import os
import sys

def dump_csf():
    filepath = "workspace/CHARSEL1.CSF"
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
        return
        
    with open(filepath, 'rb') as f:
        data = f.read(256)
        
    print("Hex dump:")
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_str = ' '.join(f"{b:02X}" for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
        print(f"{i:04X}: {hex_str:<48} | {ascii_str}")

if __name__ == "__main__":
    dump_csf()
