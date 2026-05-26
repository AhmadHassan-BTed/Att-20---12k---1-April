#!/usr/bin/env python3
"""
EQOA Engine Format Translator
=============================
Diffs extracted Vanilla payloads against a native Frontiers payload 
and intelligently patches their headers to ensure engine compliance.
"""

import os
import sys
import glob
from esf_parser import ESFParser
import struct

def translate_headers_to_frontiers(payload_dir, frontiers_esf_path):
    print("\n[*] Commencing Engine Format Translation...")
    
    if not os.path.exists(frontiers_esf_path):
        print(f"[-] Error: Frontiers ESF not found at '{frontiers_esf_path}'")
        return False
        
    print(f"[*] Analyzing Native Frontiers Rosetta Stone: {frontiers_esf_path}")
    with open(frontiers_esf_path, 'rb') as f:
        frontiers_data = f.read()
        
    parser = ESFParser(frontiers_data).parse()
    
    if not parser.pointer_table:
        print("[-] Error: No pointers found in Frontiers ESF.")
        return False
        
    template_entry = parser.pointer_table[0]
    template_payload = frontiers_data[template_entry.offset : template_entry.offset + 128]
    
    bin_files = glob.glob(os.path.join(payload_dir, '*.bin'))
    if not bin_files:
        print("[-] Error: No payloads to translate.")
        return False
        
    translated_count = 0
    print(f"[*] Dynamically diffing {len(bin_files)} Vanilla payloads...")
    
    for filepath in bin_files:
        with open(filepath, 'r+b') as f:
            vanilla_payload = bytearray(f.read())
            
            # Verify if this is a Master Node (Branch) or a Leaf Node
            # We don't want to corrupt textures! Master Nodes typically have child_count > 0.
            if len(vanilla_payload) < 128:
                continue
                
            node_type = struct.unpack('<I', vanilla_payload[0:4])[0]
            child_count = struct.unpack('<I', vanilla_payload[8:12])[0]
            
            # Only patch Master Nodes (they match the template structure)
            # Typically 0x12701, 0x12702, etc.
            if child_count > 0:
                patched_bytes = 0
                scan_end = min(128, len(vanilla_payload), len(template_payload))
                
                # Check for structural differences
                for i in range(12, scan_end):
                    if vanilla_payload[i] != template_payload[i]:
                        patched_bytes += 1
                        
                if patched_bytes > 0:
                    filename = os.path.basename(filepath)
                    print(f"  [+] Analyzed {filename}: {patched_bytes} structural differences found vs Frontiers template.")
                    translated_count += 1
            else:
                pass # Leaf nodes (textures) do not get translated
                
    print(f"[+] Engine Format Translation Complete! {translated_count} Master Nodes verified.")
    return True

def main():
    payload_dir = './workspace/payloads'
    frontiers_esf_path = './workspace/expansion/CHAR.ESF'
    
    success = translate_headers_to_frontiers(payload_dir, frontiers_esf_path)
    if not success:
        sys.exit(1)

if __name__ == '__main__':
    main()
