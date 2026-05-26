import os
import sys
import shutil
import json
import subprocess
from esf_parser import ESFParser

def main():
    json_path = 'workspace/target_assets.json'
    original_esf = 'workspace/original/CHAR.ESF'
    payloads_dir = 'workspace/payloads'
    
    print("=" * 70)
    # Persona: low level game developer doing a pristine surgery transplant
    print("  EQOA PRISTINE SURGERY TRANSPLANT PIPELINE (Untouched Original Models)")
    print("=" * 70)
    
    if not os.path.exists(json_path):
        print(f"[-] Error: {json_path} not found!")
        sys.exit(1)
        
    if not os.path.exists(original_esf):
        print(f"[-] Error: {original_esf} not found!")
        sys.exit(1)
        
    with open(json_path, 'r') as f:
        targets = json.load(f)
        
    # Clear out payloads directory
    if os.path.exists(payloads_dir):
        shutil.rmtree(payloads_dir)
    os.makedirs(payloads_dir, exist_ok=True)
    
    # Load original ESF
    print(f"\n[*] Parsing Vanilla ESF: {original_esf}...")
    with open(original_esf, 'rb') as f:
        van_esf_bytes = f.read()
    van_parser = ESFParser(van_esf_bytes).parse()
    van_map = {e.asset_id: e for e in van_parser.pointer_table if e.asset_id is not None}
    
    # Extract pristine payloads
    print(f"\n[*] Extracting the {len(targets)} pristine Vanilla character models...")
    for idx, t in enumerate(targets):
        h = int(t['original_hash'], 16)
        print(f"  [{idx+1}/11] Extracting asset 0x{h:08X}...")
        
        van_entry = van_map[h]
        payload = van_esf_bytes[van_entry.offset : van_entry.offset + van_entry.length]
        
        bin_path = os.path.join(payloads_dir, f"asset_0x{h:08X}.bin")
        with open(bin_path, 'wb') as f:
            f.write(payload)
        print(f"    [+] Saved pristine payload -> {bin_path} ({len(payload):,} bytes)")
        
    # Trigger database rebuilder
    print("\n[*] Rebuilding the merged CHAR.ESF database...")
    subprocess.run([sys.executable, "esf_rebuilder.py"], check=True)
    
    # Trigger ISO repacker
    print("\n[*] Repacking and patching the playable game ISO...")
    subprocess.run([sys.executable, "repack_iso.py"], check=True)
    
    # Verify final ISO
    print("\n[*] Verifying repacked ISO integrity...")
    subprocess.run([sys.executable, "verify_final_iso.py"], check=True)
    
    print("\n" + "=" * 70)
    print("  PRISTINE TRANSPLANT PIPELINE SUCCESSFUL!")
    print("  Original character models have been surgically transplanted into the expansion.")
    print("=" * 70)

if __name__ == '__main__':
    main()
