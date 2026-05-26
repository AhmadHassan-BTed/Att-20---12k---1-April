import os
import sys

def main():
    # Force UTF-8 stdout
    sys.stdout.reconfigure(encoding='utf-8')
    
    dump_file = 'diagnostics/live_memory_dump.txt'
    if not os.path.exists(dump_file):
        print("live_memory_dump.txt not found!")
        return
        
    print(f"Reading {dump_file}...")
    with open(dump_file, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()
        
    print("\n=== Search Results for 0xCD51EF83 in live_memory_dump.txt ===")
    lines = content.split('\n')
    for idx, line in enumerate(lines):
        if "CD51EF83" in line or "cd51ef83" in line:
            start = max(0, idx - 15)
            end = min(len(lines), idx + 15)
            print(f"\n--- Occurrence at line {idx+1} ---")
            print('\n'.join(lines[start:end]))
            print("-" * 50)

if __name__ == '__main__':
    main()
