#!/usr/bin/env python3
"""
Check which files PCSX2 has open and their sizes.
Helps diagnose which ISO file PCSX2 is actually reading from.
"""
import subprocess
import os
import sys

def check_pcsx2_open_files():
    print("\n[CHECK] PCSX2 Open Files")
    print("=" * 70)

    try:
        # Get PCSX2 process IDs
        result = subprocess.run(
            ['powershell', '-Command', 'Get-Process -Name pcsx2* -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Id'],
            capture_output=True,
            text=True,
            timeout=5
        )

        pids = [int(x.strip()) for x in result.stdout.strip().split('\n') if x.strip() and x.strip().isdigit()]

        if not pids:
            print("\n[INFO] PCSX2 is not currently running")
            print("       Cannot check open files")
            return False

        print(f"\nFound {len(pids)} PCSX2 process(es)")

        for pid in pids:
            print(f"\n--- Process ID {pid} ---")
            try:
                # Use PowerShell to check open handles
                cmd = f'Get-Process -Id {pid} -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Name'
                result = subprocess.run(['powershell', '-Command', cmd], capture_output=True, text=True, timeout=5)
                proc_name = result.stdout.strip() if result.stdout else "Unknown"
                print(f"Process: {proc_name}")

                # Try to find open ISO files via lsof or similar (Windows doesn't have lsof)
                # Alternative: check process working directory and modules
                cmd = f'Get-Process -Id {pid} 2>/dev/null | Get-ProcessOpenFile 2>/dev/null'
                # This won't work on all systems, so just check common locations
                iso_files = [
                    'iso/patched/EQOA_Frontiers_Patched.iso',
                    'iso/unmodified/EQOA_Frontiers.iso',
                    'iso/unmodified/EQOA_Original.iso',
                ]

                print("\nChecking ISO file accessibility:")
                for iso_path in iso_files:
                    try:
                        if os.path.exists(iso_path):
                            size = os.path.getsize(iso_path)
                            try:
                                # Try to open and lock it
                                with open(iso_path, 'r+b') as f:
                                    print(f"  {iso_path}: {size:,} bytes [ACCESSIBLE]")
                            except PermissionError:
                                print(f"  {iso_path}: {size:,} bytes [LOCKED BY PROCESS]")
                        else:
                            print(f"  {iso_path}: NOT FOUND")
                    except Exception as e:
                        print(f"  {iso_path}: ERROR - {e}")
            except Exception as e:
                print(f"  Error querying process: {e}")

        return True

    except subprocess.TimeoutExpired:
        print("[ERROR] PowerShell command timed out")
        return False
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

if __name__ == '__main__':
    check_pcsx2_open_files()
