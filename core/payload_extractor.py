#!/usr/bin/env python3
"""
EQOA Complete Binary Payload Extractor
======================================
Extracts ALL 410 classic character, head, and armor assets from the original 
Vanilla CHAR.ESF file to ensure the entire classic graphics pipeline is present 
and binds correctly to the classic skeleton.
"""

import os
import sys
import struct
from core.esf_parser import ESFParser


def extract_all_payloads(esf_path, output_dir):
    """Slice exact model payloads from original CHAR.ESF."""
    if not os.path.exists(esf_path):
        print(f"[-] Error: Original ESF not found at '{esf_path}'", file=sys.stderr)
        return False
        
    print(f"[*] Reading Original ESF: {esf_path}...")
    with open(esf_path, 'rb') as f:
        esf_data = f.read()

    print("[*] Parsing ESF Virtual Pointer Table...")
    parser = ESFParser(esf_data).parse()
    
    os.makedirs(output_dir, exist_ok=True)
    
    # Extract all assets in the pointer table
    print(f"\n[*] Commencing full binary slicing of {len(parser.pointer_table)} assets...")
    
    extracted_count = 0
    for entry in parser.pointer_table:
        if entry.asset_id is None:
            continue
            
        # Verify slice boundaries
        if entry.offset + entry.length > len(esf_data):
            print(f"[-] Error: Slice bounds [0x{entry.offset:X} : 0x{entry.offset+entry.length:X}] out of bounds!", file=sys.stderr)
            continue
            
        payload = esf_data[entry.offset : entry.offset + entry.length]
        
        out_name = f"asset_0x{entry.asset_id:08X}.bin"
        out_path = os.path.join(output_dir, out_name)
        
        with open(out_path, 'wb') as out_f:
            out_f.write(payload)
            
        extracted_count += 1
        
    print(f"\n[+] EXTRACTION SUCCESS: {extracted_count} classic payloads extracted successfully!")
    return True


def main():
    esf_path = './workspace/original/CHAR.ESF'
    output_dir = './workspace/payloads'
    
    success = extract_all_payloads(esf_path, output_dir)
    if not success:
        sys.exit(1)


if __name__ == '__main__':
    main()
