#!/usr/bin/env python3
import os
import shutil

def refactor():
    root_dir = os.path.dirname(os.path.abspath(__file__))
    
    docs_dir = os.path.join(root_dir, 'docs')
    core_dir = os.path.join(root_dir, 'core')
    legacy_dir = os.path.join(root_dir, 'legacy')
    
    os.makedirs(docs_dir, exist_ok=True)
    os.makedirs(core_dir, exist_ok=True)
    os.makedirs(legacy_dir, exist_ok=True)
    
    # Docs files
    docs_files = [
        'architecture.md',
        'tech_stack.md',
        'todo.md',
        'progress.md',
        'tasks.md',
        'walkthrough.md'
    ]
    
    # Core files
    core_files = [
        'esf_parser.py',
        'esf_rebuilder.py',
        'repack_iso.py',
        'patch_udf_char_esf_v2.py',
        'verify_final_patch.py',
        'verify_final_iso.py',
        'verify_injected_models.py'
    ]
    
    moved_count = 0
    
    # Move docs from root or other folders to docs/
    for filename in docs_files:
        src = os.path.join(root_dir, filename)
        if os.path.exists(src):
            dst = os.path.join(docs_dir, filename)
            shutil.move(src, dst)
            print(f"[+] Moved doc: {filename} -> docs/")
            moved_count += 1
            
    # Move core files from root to core/
    for filename in core_files:
        src = os.path.join(root_dir, filename)
        if os.path.exists(src):
            dst = os.path.join(core_dir, filename)
            shutil.move(src, dst)
            print(f"[+] Moved core script: {filename} -> core/")
            moved_count += 1
            
    # Also move any docs currently in the root to docs/
    # Move organize_repo.py to legacy
    src_org = os.path.join(root_dir, 'organize_repo.py')
    if os.path.exists(src_org):
        shutil.move(src_org, os.path.join(legacy_dir, 'organize_repo.py'))
        print("[+] Moved organize_repo.py -> legacy/")
        
    print(f"\n[SUCCESS] Refactored root into architectural layers: docs/ and core/ ({moved_count} items relocated).")

if __name__ == '__main__':
    refactor()
