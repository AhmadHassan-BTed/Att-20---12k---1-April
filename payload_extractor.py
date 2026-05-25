#!/usr/bin/env python3
"""
EQOA Binary Payload Extractor
=============================
Reads target_assets.json, loads the original CHAR.ESF file, slices the 
raw binary chunks of the 11 target player models, and saves them strictly 
according to their lengths into the ./workspace/payloads/ directory.
"""

import os
import sys
import json

def extract_payloads(json_path, esf_path, output_dir):
    """Slice the exact model payloads from original CHAR.ESF and verify sizes."""
    # 1. Load target assets JSON
    if not os.path.exists(json_path):
        print(f"[-] Error: Could not locate target assets map at '{json_path}'", file=sys.stderr)
        return False
        
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    print(f"[*] Loaded target assets map. Total assets to extract: {len(targets)}")

    # 2. Load Original CHAR.ESF
    if not os.path.exists(esf_path):
        print(f"[-] Error: Original ESF not found at '{esf_path}'", file=sys.stderr)
        return False
        
    print(f"[*] Reading Original ESF: {esf_path}...")
    with open(esf_path, 'rb') as f:
        esf_data = f.read()

    # 3. Create output payloads folder
    os.makedirs(output_dir, exist_ok=True)
    
    extracted_summary = []
    
    # 4. Extract each payload sequentially
    print("\n[*] Commencing precise binary slicing...")
    for idx, t in enumerate(targets):
        h = t['original_hash']
        offset = t['original_offset']
        length = t['original_length']
        
        # Verify slice parameters do not exceed file boundary
        if offset + length > len(esf_data):
            print(f"[-] Error: Target slice bounds [0x{offset:X} : 0x{offset+length:X}] out of bounds of ESF data", file=sys.stderr)
            continue
            
        # Raw binary slicing (strictly obeys original_length)
        payload = esf_data[offset : offset + length]
        
        out_name = f"asset_{h}.bin"
        out_path = os.path.join(output_dir, out_name)
        
        # Write to payload binary file
        with open(out_path, 'wb') as out_f:
            out_f.write(payload)
            
        physical_size = os.path.getsize(out_path)
        mismatch = "PASS" if physical_size == length else "FAIL"
        
        extracted_summary.append((out_name, length, physical_size, mismatch))
        print(f"  [{idx+1:2d}/11] Extracted {out_name} | Offset: 0x{offset:X} | Expected: {length:,} B | Sliced: {physical_size:,} B | Verification: {mismatch}")

    # 5. Output Verification Report
    print("\n=== Slicing Verification Report ===")
    print(f"{'Filename':<28} | {'Expected Size (B)':<18} | {'Physical Size (B)':<18} | {'Status':<6}")
    print(f"{'-'*28}-|-{'-'*18}-|-{'-'*18}-|-{'-'*6}")
    
    all_passed = True
    for name, expected, physical, status in extracted_summary:
        print(f"{name:<28} | {expected:<18,} | {physical:<18,} | {status:<6}")
        if status != "PASS":
            all_passed = False
            
    print("-" * 76)
    if all_passed and len(extracted_summary) == 11:
        print("[+] VERIFICATION SUCCESS: All 11 models extracted with 100% byte-perfect precision!")
        return True
    else:
        print("[-] VERIFICATION FAILURE: Some extraction sizes mismatched or incomplete.")
        return False

def main():
    json_path = './workspace/target_assets.json'
    esf_path = './workspace/original/CHAR.ESF'
    output_dir = './workspace/payloads'
    
    success = extract_payloads(json_path, esf_path, output_dir)
    if not success:
        sys.exit(1)

if __name__ == '__main__':
    main()
