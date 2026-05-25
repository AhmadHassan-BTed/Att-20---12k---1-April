#!/usr/bin/env python3
"""
EQOA ESF Hex Analyzer
======================
Reads the first 512 bytes of the extracted CHAR.ESF file and outputs a 
perfectly formatted 16-byte hex dump with ASCII decoding.
"""

import os
import sys

def hex_dump(filepath, num_bytes=512):
    """Generate and display a formatted hex dump of the given file."""
    if not os.path.exists(filepath):
        print(f"[-] Error: File not found at '{filepath}'", file=sys.stderr)
        return False
        
    print(f"[*] Analyzing binary structure: {filepath}")
    print(f"[*] Reading first {num_bytes} bytes...\n")
    
    with open(filepath, 'rb') as f:
        data = f.read(num_bytes)
        
    print(f"{'Offset':>8}   {'Hexadecimal Bytes':<47}   {'ASCII'}")
    print(f"{'-'*8}   {'-'*47}   {'-'*16}")
    
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        # Hex representation
        hex_str = ' '.join(f"{b:02X}" for b in chunk)
        # ASCII representation
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
        
        # Output layout
        print(f"0x{i:06X}   {hex_str:<47}   {ascii_str}")
        
    print(f"\n[+] Total bytes read: {len(data)}")
    return True

def main():
    # Check both likely workspace paths to be resilient
    paths = [
        './workspace/original/CHAR.ESF',
        './workspace/original/CHAR.ESF',
        'original/CHAR.ESF'
    ]
    
    filepath = None
    for p in paths:
        if os.path.exists(p):
            filepath = p
            break
            
    if not filepath:
        print("[-] Error: Could not locate original/CHAR.ESF in './workspace/original/'", file=sys.stderr)
        sys.exit(1)
        
    hex_dump(filepath, 512)

if __name__ == '__main__':
    main()
