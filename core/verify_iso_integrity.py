import sys
import hashlib

def sha256_file(filepath):
    sha256 = hashlib.sha256()
    try:
        with open(filepath, 'rb') as f:
            # Read in 64k chunks
            for chunk in iter(lambda: f.read(65536), b''):
                sha256.update(chunk)
        return sha256.hexdigest()
    except Exception as e:
        return f"Error: {e}"

def main():
    unmod_iso = "iso/unmodified/EQOA_Frontiers.iso"
    patch_iso = "iso/patched/EQOA_Frontiers_Patched.iso"

    print("Executing Step 1: The Hash Audit...")
    
    hash_unmod = sha256_file(unmod_iso)
    print(f"Unmodified ISO Hash: {hash_unmod}")
    
    hash_patch = sha256_file(patch_iso)
    print(f"Patched ISO Hash:    {hash_patch}")

    if hash_unmod == hash_patch:
        print("\n[FAILURE] The hashes are identical. The rebuilder is NOT patching the ISO.")
    else:
        print("\n[SUCCESS] The hashes are different. Proceeding to Step 2.")

if __name__ == "__main__":
    main()
