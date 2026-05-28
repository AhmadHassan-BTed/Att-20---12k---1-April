import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from core.esf_parser import ESFParser
import zlib

def decompress_csf(filepath):
    with open(filepath, 'rb') as f:
        data = f.read()
    if not data.startswith(b'CESF'):
        return data
    out_buf = bytearray()
    pos = 0
    while pos < len(data):
        idx = data.find(b'\x78\xda', pos)
        if idx == -1: break
        try:
            d = zlib.decompressobj()
            out_buf.extend(d.decompress(data[idx:]))
            pos = idx + (len(data[idx:]) - len(d.unused_data))
        except:
            break
    return bytes(out_buf)

filepath = "workspace/CHARCUST_Original.CSF"
data = decompress_csf(filepath)
parser = ESFParser(data).parse()
print(f"Pointer table entries: {len(parser.pointer_table)}")
for entry in parser.pointer_table:
    if entry.asset_id:
        print(f"0x{entry.asset_id:08X} - {entry.type_id:05X}")
