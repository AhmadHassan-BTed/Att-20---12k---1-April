#!/usr/bin/env python3
"""
Master wrapper script for running all diagnostic checks.
Provides a single entry point to run all diagnostics and generate a comprehensive report.
"""
import subprocess
import sys
import os

class MasterDiagnosticRunner:
    def __init__(self):
        self.scripts = [
            {
                'name': 'Integrated Diagnostics',
                'script': 'integrated_diagnostics.py',
                'description': 'Complete ISO, ESF, and PCSX2 state verification'
            },
            {
                'name': 'ISO Patch Verification',
                'script': 'workspace/scratch/check_iso_appended_bytes.py',
                'description': 'Verify patched data was written to correct ISO location'
            },
            {
                'name': 'ESF Asset Comparison',
                'script': 'workspace/scratch/compare_esf_sizes.py',
                'description': 'Compare Vanilla, Frontiers, and Patched model sizes'
            },
            {
                'name': 'PCSX2 File Access',
                'script': 'workspace/scratch/check_pcsx2_open_files.py',
                'description': 'Check which ISO files PCSX2 can access'
            },
        ]
        self.results = {}

    def run_all_diagnostics(self):
        """Run all diagnostic scripts and collect results"""
        print("\n" + "=" * 70)
        print("  MASTER DIAGNOSTIC RUNNER - EQOA Character Visibility")
        print("=" * 70)

        for i, script_info in enumerate(self.scripts, 1):
            print(f"\n[{i}/{len(self.scripts)}] Running: {script_info['name']}")
            print(f"    Description: {script_info['description']}")
            print("-" * 70)

            try:
                result = subprocess.run(
                    [sys.executable, script_info['script']],
                    capture_output=False,
                    text=True,
                    timeout=60,
                    cwd=os.getcwd()
                )
                self.results[script_info['name']] = {
                    'status': 'SUCCESS' if result.returncode == 0 else 'FAILED',
                    'returncode': result.returncode
                }
            except subprocess.TimeoutExpired:
                print(f"\n[TIMEOUT] Script exceeded 60 second timeout")
                self.results[script_info['name']] = {'status': 'TIMEOUT', 'returncode': -1}
            except Exception as e:
                print(f"\n[ERROR] {e}")
                self.results[script_info['name']] = {'status': 'ERROR', 'error': str(e)}

        self.print_summary()

    def print_summary(self):
        """Print diagnostic summary"""
        print("\n" + "=" * 70)
        print("  DIAGNOSTIC SUMMARY")
        print("=" * 70)

        all_passed = True
        for script_name, result in self.results.items():
            status_str = result['status']
            if result['status'] != 'SUCCESS':
                all_passed = False
                print(f"[{result['status']}] {script_name}")
            else:
                print(f"[OK] {script_name}")

        print("\n" + "-" * 70)
        if all_passed:
            print("[FINAL RESULT] All diagnostics passed successfully")
            print("\nRECOMMENDATION:")
            print("  1. Close PCSX2 completely")
            print("  2. Relaunch PCSX2 fresh (no savestate resume)")
            print("  3. Load: iso/patched/EQOA_Frontiers_Patched.iso")
            print("  4. Connect to Sandstorm server and spawn a character")
            print("\nCharacter should now be FULLY VISIBLE")
        else:
            print("[FINAL RESULT] Some diagnostics require attention")
            print("\nRECOMMENDATION:")
            print("  Review the diagnostic output above for issues")
            print("  Common solutions:")
            print("    - Ensure ISO files are in: iso/patched/ and iso/unmodified/")
            print("    - Verify PCSX2 is closed (no lingering processes)")
            print("    - Check disk space and file permissions")

        print("\n" + "=" * 70 + "\n")

if __name__ == '__main__':
    runner = MasterDiagnosticRunner()
    runner.run_all_diagnostics()
