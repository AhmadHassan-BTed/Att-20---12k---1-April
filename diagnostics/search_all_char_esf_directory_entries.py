with open('EQOA_Frontiers.iso', 'rb') as f:
    data = f.read()

import re
matches = [m.start() for m in re.finditer(b'CHAR.ESF', data, re.IGNORECASE)]

print(f"[+] Found {len(matches)} occurrences of 'CHAR.ESF' (case-insensitive):")
for idx, pos in enumerate(matches):
    surround = data[max(0, pos-32) : min(len(data), pos+32)]
    print(f"  [{idx}]: Offset 0x{pos:X} | Context: {surround.hex()} | ASCII: {surround}")
