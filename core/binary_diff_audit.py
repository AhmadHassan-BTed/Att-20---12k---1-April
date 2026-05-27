import sys
import struct
import os

def load_payload_from_esf(esf_path, target_hash):
    # Just a standalone parser to pull the full payload bytes out of CHAR.ESF
    from core.esf_parser import ESFParser
    with open(esf_path, 'rb') as f:
        data = f.read()
    parser = ESFParser(data).parse()
    for entry in parser.pointer_table:
        if entry.asset_id == target_hash:
            return data[entry.offset : entry.offset + entry.length]
    raise Exception(f"Asset 0x{target_hash:08X} not found in {esf_path}")

def hex_diff(golden, broken, limit=512):
    golden = golden[:limit]
    broken = broken[:limit]
    
    print("="*80)
    print(" BINARY DIFF AUDIT REPORT (FIRST 512 BYTES)")
    print("="*80)
    
    # Print side by side
    print(f"{'OFFSET':<8} | {'GOLDEN MASTER (Native Frontiers)':<48} | {'INJECTED PAYLOAD (Broken)':<48} | {'DIFF'}")
    print("-" * 115)
    
    differences_found = False
    
    for i in range(0, max(len(golden), len(broken)), 16):
        g_chunk = golden[i:i+16]
        b_chunk = broken[i:i+16]
        
        g_hex = " ".join([f"{b:02X}" for b in g_chunk])
        b_hex = " ".join([f"{b:02X}" for b in b_chunk])
        
        # Pad strings for formatting
        g_hex = f"{g_hex:<47}"
        b_hex = f"{b_hex:<47}"
        
        if g_chunk != b_chunk:
            diff_marker = "<-- MISMATCH"
            differences_found = True
        else:
            diff_marker = "MATCH"
            
        print(f"0x{i:04X}   | {g_hex} | {b_hex} | {diff_marker}")
        
    print("-" * 115)
    if differences_found:
        print("[!] Differences detected in the header block.")
    else:
        print("[PASS] The first 512 bytes are mathematically identical.")

def main():
    target_hash = 0x05AEBA67
    
    try:
        print(f"[*] Extracting Golden Master Frontiers Payload for 0x{target_hash:08X}...")
        golden_bytes = load_payload_from_esf('workspace/expansion/CHAR.ESF', target_hash)
        
        broken_path = f"workspace/payloads/asset_0x{target_hash:08X}.bin"
        print(f"[*] Loading Injected Payload: {broken_path}...")
        with open(broken_path, 'rb') as f:
            broken_bytes = f.read()
            
        hex_diff(golden_bytes, broken_bytes, 512)
    except Exception as e:
        print(f"[-] Error: {e}")

if __name__ == "__main__":
    sys.path.append(os.getcwd())
    main()
