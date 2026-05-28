import os
import struct

def search_manifest(filepath, target_ids):
    if not os.path.exists(filepath):
        return
        
    with open(filepath, 'rb') as f:
        data = f.read()
        
    print(f"\n[*] Scanning {os.path.basename(filepath)} ({len(data)} bytes)...")
    
    found_any = False
    for asset_id in target_ids:
        # Search for Little-Endian
        le_bytes = struct.pack('<I', asset_id)
        # Search for Big-Endian
        be_bytes = struct.pack('>I', asset_id)
        
        idx = 0
        while True:
            idx = data.find(le_bytes, idx)
            if idx == -1: break
            
            found_any = True
            # Dump context (32 bytes before and 32 bytes after)
            start = max(0, idx - 32)
            end = min(len(data), idx + 32)
            context = data[start:end]
            
            print(f"  [+] Found LE Match for 0x{asset_id:08X} at offset 0x{idx:08X}")
            
            # Print hex dump
            lines = []
            for i in range(0, len(context), 16):
                chunk = context[i:i+16]
                hex_str = " ".join([f"{b:02X}" for b in chunk])
                ascii_str = "".join([chr(b) if 32 <= b <= 126 else '.' for b in chunk])
                lines.append(f"      0x{start+i:08X}: {hex_str:<48} | {ascii_str}")
                
            for line in lines:
                print(line)
            print()
            idx += 4

        idx = 0
        while True:
            idx = data.find(be_bytes, idx)
            if idx == -1: break
            
            found_any = True
            start = max(0, idx - 32)
            end = min(len(data), idx + 32)
            context = data[start:end]
            
            print(f"  [+] Found BE Match for 0x{asset_id:08X} at offset 0x{idx:08X}")
            lines = []
            for i in range(0, len(context), 16):
                chunk = context[i:i+16]
                hex_str = " ".join([f"{b:02X}" for b in chunk])
                ascii_str = "".join([chr(b) if 32 <= b <= 126 else '.' for b in chunk])
                lines.append(f"      0x{start+i:08X}: {hex_str:<48} | {ascii_str}")
                
            for line in lines:
                print(line)
            print()
            idx += 4
            
    if not found_any:
        print("  [-] No matches found.")

def main():
    workspace = 'workspace'
    target_ids = [0x05AEBA67, 0x2EF8E480, 0x0017A0BD]  # Sample of our injected character hashes
    
    manifests = [
        'STATION.AUT',
        'CLIENT.AUT',
        'SUPPORT.AUT',
        'CHARSEL1.ESF', 'CHARSEL1.CSF',
        'CHARSEL2.ESF', 'CHARSEL2.CSF',
        'CHARSEL3.ESF', 'CHARSEL3.CSF',
        'CHARSEL4.ESF', 'CHARSEL4.CSF'
    ]
    
    print("================================================================================")
    print(" MANIFEST REDIRECT AUDIT")
    print("================================================================================")
    for m in manifests:
        search_manifest(os.path.join(workspace, m), target_ids)

if __name__ == "__main__":
    main()
