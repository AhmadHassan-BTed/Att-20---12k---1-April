#!/usr/bin/env python3
"""
EQOA Hex Analyzer - Deep Diagnostic Edition
===========================================
Executes 4 verification phases to diagnose invisible PS2 character meshes.
"""

import os
import sys
import struct
import glob

def phase1_vif_dma_validation(payload_dir):
    print("\n" + "="*50)
    print("PHASE 1: Binary VIF/DMA Tag Validation")
    print("="*50)
    
    bin_files = glob.glob(os.path.join(payload_dir, '*.bin'))
    if not bin_files:
        print("[-] No .bin payloads found in workspace/payloads")
        return
        
    for filepath in bin_files:
        filename = os.path.basename(filepath)
        with open(filepath, 'rb') as f:
            data = f.read()
            
        vif_unpack_count = 0
        vif_stcycl_count = 0
        
        # Scan for VIFcode UNPACK (usually 0x60-0x7F in the MSB)
        # e.g., 0x60000000 to 0x7FFFFFFF in little endian
        for i in range(0, len(data) - 4, 4):
            val = struct.unpack('<I', data[i:i+4])[0]
            # VIFcode UNPACK is 0x60000000 | (vl << 16) | (vn << 8) | addr
            if (val & 0xFF000000) >= 0x60000000 and (val & 0xFF000000) <= 0x7F000000:
                vif_unpack_count += 1
            # VIFcode STCYCL is 0x30000000
            elif (val & 0xFF000000) == 0x30000000:
                vif_stcycl_count += 1
                
        status = "PASS" if vif_unpack_count > 0 else "FAIL"
        print(f"[{status}] {filename:<20} | Size: {len(data):>8} B | UNPACK Tags: {vif_unpack_count:>4} | STCYCL Tags: {vif_stcycl_count:>4}")

def phase2_asset_dependency_mapping(payload_dir):
    print("\n" + "="*50)
    print("PHASE 2: Asset Dependency Mapping")
    print("="*50)
    
    bin_files = glob.glob(os.path.join(payload_dir, '*.bin'))
    for filepath in bin_files:
        filename = os.path.basename(filepath)
        with open(filepath, 'rb') as f:
            # Read just the header area (e.g. first 256 bytes)
            header_data = f.read(256)
            
        print(f"\n[*] Scanning {filename} header for potential Asset IDs...")
        
        # Look for arrays of 32-bit integers that might be Hashes
        found_ids = []
        for i in range(0, len(header_data) - 4, 4):
            val = struct.unpack('<I', header_data[i:i+4])[0]
            # EQOA Asset IDs are typically randomly distributed 32-bit hashes
            # We filter out very small numbers or common flags
            if val > 0x00010000 and val != 0xFFFFFFFF:
                found_ids.append(f"0x{val:08X}")
                
        print(f"    Potential 32-bit Dependencies: {', '.join(found_ids[:10])} ...")

def phase3_internal_pointer_auditing(payload_dir):
    print("\n" + "="*50)
    print("PHASE 3: Internal Pointer Auditing")
    print("="*50)
    
    bin_files = glob.glob(os.path.join(payload_dir, '*.bin'))
    for filepath in bin_files:
        filename = os.path.basename(filepath)
        with open(filepath, 'rb') as f:
            header_data = f.read(128)
            
        absolute_count = 0
        relative_count = 0
        
        for i in range(0, len(header_data) - 4, 4):
            val = struct.unpack('<I', header_data[i:i+4])[0]
            # Absolute offsets into the 148MB ESF would be large (e.g. > 0x00100000)
            if 0x00100000 < val < 0x0FFFFFFF:
                absolute_count += 1
            # Relative offsets would be strictly within the 500KB payload
            elif 0x00000020 < val < 0x000F0000:
                relative_count += 1
                
        print(f"[*] {filename:<20} | Potential Absolute Pointers: {absolute_count} | Potential Relative Pointers: {relative_count}")

def phase4_engine_format_diffing(vanilla_bin, frontiers_bin):
    print("\n" + "="*50)
    print("PHASE 4: Engine Format Diffing (Vanilla vs Frontiers)")
    print("="*50)
    
    if not os.path.exists(vanilla_bin) or not os.path.exists(frontiers_bin):
        print(f"[-] Required .bin files for diffing not found.")
        print(f"    Expected: {vanilla_bin}")
        print(f"    Expected: {frontiers_bin}")
        return
        
    with open(vanilla_bin, 'rb') as f1, open(frontiers_bin, 'rb') as f2:
        data1 = f1.read(256)
        data2 = f2.read(256)
        
    print(f"[*] Structurally Diffing First 256 Bytes:")
    print(f"    Vanilla:   {vanilla_bin}")
    print(f"    Frontiers: {frontiers_bin}\n")
    
    print(f"{'Offset':>8} | {'Vanilla Hex':<23} | {'Frontiers Hex':<23} | Diff")
    print("-" * 75)
    
    min_len = min(len(data1), len(data2))
    for i in range(0, min_len, 8):
        chunk1 = data1[i:i+8]
        chunk2 = data2[i:i+8]
        
        hex1 = ' '.join(f"{b:02X}" for b in chunk1)
        hex2 = ' '.join(f"{b:02X}" for b in chunk2)
        
        match = "==" if chunk1 == chunk2 else "<>"
        
        print(f"0x{i:06X} | {hex1:<23} | {hex2:<23} | {match}")

def main():
    payload_dir = './workspace/payloads'
    
    # Example paths for Phase 4 (client can update these to real extracted paths)
    vanilla_sample = './workspace/payloads/asset_0x05AEBA67.bin'
    frontiers_sample = './workspace/payloads/frontiers_asset_sample.bin'
    
    phase1_vif_dma_validation(payload_dir)
    phase2_asset_dependency_mapping(payload_dir)
    phase3_internal_pointer_auditing(payload_dir)
    phase4_engine_format_diffing(vanilla_sample, frontiers_sample)
    
    print("\n[+] Diagnostic Routines Complete. Review output for missing dependencies or header discrepancies.")

if __name__ == '__main__':
    main()
