#!/usr/bin/env python3
"""
EQOA Safe ISO Asset Extractor
==============================
Extracts CHAR.ESF and CHAR.CSF from EverQuest Online Adventures ISO images 
using pycdlib. Avoids standard OS mounting or extraction tool corruptions.
"""

import os
import sys
import hashlib
import pycdlib

def calculate_sha256(filepath):
    """Calculate the SHA-256 hash of a file."""
    sha256 = hashlib.sha256()
    with open(filepath, 'rb') as f:
        while True:
            chunk = f.read(1024 * 1024)
            if not chunk:
                break
            sha256.update(chunk)
    return sha256.hexdigest()

def extract_file_from_iso(iso_path, internal_path, output_path):
    """Extract a specific file from an ISO to the output path using pycdlib."""
    iso = pycdlib.PyCdlib()
    try:
        iso.open(iso_path)
    except Exception as e:
        print(f"[-] Error opening ISO '{iso_path}': {e}", file=sys.stderr)
        return False

    success = False
    try:
        print(f"[*] Extracting '{internal_path}' from '{iso_path}'...")
        with open(output_path, 'wb') as out_f:
            iso.get_file_from_iso_fp(out_f, iso_path=internal_path)
        
        file_size = os.path.getsize(output_path)
        print(f"[+] Successfully extracted to '{output_path}' ({file_size:,} bytes)")
        success = True
    except Exception as e:
        # Check if it was just a missing optional file
        print(f"[-] Could not extract '{internal_path}': {e}", file=sys.stderr)
        if os.path.exists(output_path):
            os.remove(output_path)
    finally:
        iso.close()
    
    return success

def main():
    original_iso = 'EQOA_Original.iso'
    expansion_iso = 'EQOA_Frontiers.iso'
    
    original_dir = './workspace/original'
    expansion_dir = './workspace/expansion'
    
    # Ensure directories exist
    os.makedirs(original_dir, exist_ok=True)
    os.makedirs(expansion_dir, exist_ok=True)
    
    targets = [
        # (ISO, internal path, output path)
        (original_iso, '/DATA/CHAR.ESF;1', os.path.join(original_dir, 'CHAR.ESF')),
        (original_iso, '/DATA2/CHAR.CSF;1', os.path.join(original_dir, 'CHAR.CSF')),
        (expansion_iso, '/DATA/CHAR.ESF;1', os.path.join(expansion_dir, 'CHAR.ESF')),
        (expansion_iso, '/DATA2/CHAR.CSF;1', os.path.join(expansion_dir, 'CHAR.CSF')),
    ]
    
    extracted_files = []
    
    print("=== Starting Programmatic ISO Asset Extraction ===")
    for iso, internal, out in targets:
        if not os.path.exists(iso):
            print(f"[-] Warning: ISO file '{iso}' not found in current directory.", file=sys.stderr)
            continue
            
        success = extract_file_from_iso(iso, internal, out)
        if success:
            sha256_hash = calculate_sha256(out)
            size = os.path.getsize(out)
            extracted_files.append((out, size, sha256_hash))
            
    print("\n=== Extraction Summary ===")
    if not extracted_files:
        print("No files were extracted successfully.")
    else:
        for path, size, sha in extracted_files:
            print(f"File: {path}")
            print(f"  Size: {size:,} bytes")
            print(f"  SHA-256: {sha}")
            print()

if __name__ == '__main__':
    main()
