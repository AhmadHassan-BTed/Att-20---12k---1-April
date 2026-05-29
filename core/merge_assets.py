import os
import shutil

def merge_assets():
    print("=" * 80)
    print("  EQOA ASSET MERGER PIPELINE (assets/Vanilla + assets/Frontiers)")
    print("=" * 80)
    
    vanilla_dir = 'assets/Vanilla'
    frontiers_dir = 'assets/Frontiers'
    merged_dir = 'assets/merged-assets'
    
    # 1. Clean and recreate assets/merged-assets folder
    if os.path.exists(merged_dir):
        shutil.rmtree(merged_dir)
    os.makedirs(merged_dir)
    
    # Helper function to copy folders recursively, ignoring placeholders
    def copy_assets_recursive(src, dst, is_frontiers_overlay=False):
        if not os.path.exists(src):
            return
        for root, dirs, files in os.walk(src):
            rel_path = os.path.relpath(root, src)
            target_dir = os.path.join(dst, rel_path) if rel_path != '.' else dst
            os.makedirs(target_dir, exist_ok=True)
            
            for file in files:
                if file == '.gitkeep':
                    continue
                if is_frontiers_overlay and file == 'CHAR.ESF':
                    print(f"  [*] Skipping baseline overlay of: {file} (Preserving Vanilla recompiled database)")
                    continue
                src_file = os.path.join(root, file)
                dst_file = os.path.join(target_dir, file)
                shutil.copy2(src_file, dst_file)
                print(f"  [+] Merged: {src_file} -> {dst_file}")
                
    # 2. Copy Vanilla-assets first (baseline)
    print("\n[*] Copying baseline Vanilla-assets...")
    copy_assets_recursive(vanilla_dir, merged_dir, is_frontiers_overlay=False)
    
    # 3. Overlay Frontiers-assets if they exist
    print("\n[*] Overlaying Frontiers-assets (if present)...")
    copy_assets_recursive(frontiers_dir, merged_dir, is_frontiers_overlay=True)
    
    print("\n[+] Asset merge complete! Final merged assets reside in 'assets/merged-assets/' directory.")
    print("=" * 80)

if __name__ == '__main__':
    merge_assets()
