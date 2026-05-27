#!/usr/bin/env python3
"""
diagnostic_suite.py
===================
Consolidated Diagnostic Suite for EQOA Live RAM Analysis.
Aggregates and executes all individual memory tracer tools under a single 
consolidated console manager, generating a master report for easy copying.

Author: Antigravity AI Coding Assistant
"""

import os
import sys
import subprocess
import time
from datetime import datetime

REPORT_PATH = "diagnostics/logs/consolidated_diagnostics_log.txt"

def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def log_and_print(lines, text):
    print(text)
    lines.append(text)

def run_script(script_path, args=[]):
    """Executes a sub-script and captures its output in real-time."""
    cmd = [sys.executable, script_path] + args
    print(f"\n[*] Launching: {' '.join(cmd)}")
    print("-" * 80)
    
    output_lines = []
    process = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1
    )
    
    # Read output line-by-line in real-time
    while True:
        line = process.stdout.readline()
        if not line and process.poll() is not None:
            break
        if line:
            sys.stdout.write(line)
            sys.stdout.flush()
            output_lines.append(line.rstrip())
            
    rc = process.poll()
    print("-" * 80)
    print(f"[+] Execution completed (Exit Code: {rc})")
    return "\n".join(output_lines)

def run_all_diagnostics():
    report_lines = []
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    log_and_print(report_lines, "=" * 80)
    log_and_print(report_lines, f"  EQOA LIVE CONSOLIDATED DIAGNOSTIC SUITE — REPORT GENERATED: {timestamp}")
    log_and_print(report_lines, "=" * 80)
    
    # 1. EEmem Base Finder
    log_and_print(report_lines, "\n" + "=" * 80)
    log_and_print(report_lines, "  [1/4] DETECTING EE RAM BASE (test_eemem_find.py)")
    log_and_print(report_lines, "=" * 80 + "\n")
    out = run_script("diagnostics/test_eemem_find.py")
    report_lines.append(out)
    
    # 2. Model Tree Comparator
    log_and_print(report_lines, "\n" + "=" * 80)
    log_and_print(report_lines, "  [2/4] MODEL TREE STRUCTURAL COMPARATOR (compare_loaded_roots.py)")
    log_and_print(report_lines, "=" * 80 + "\n")
    out = run_script("diagnostics/compare_loaded_roots.py")
    report_lines.append(out)
    
    # 3. RAM Transition Dumper
    log_and_print(report_lines, "\n" + "=" * 80)
    log_and_print(report_lines, "  [3/4] ACTIVE RAM TRANSITION DUMPER (dump_active_b070.py)")
    log_and_print(report_lines, "=" * 80 + "\n")
    if os.path.exists("workspace/scratch/dump_active_b070.py"):
        out = run_script("workspace/scratch/dump_active_b070.py")
    else:
        out = "[-] Transition dumper script not found in workspace/scratch/"
        print(out)
    report_lines.append(out)
    
    # 4. Live RAM Continuous Scanner
    log_and_print(report_lines, "\n" + "=" * 80)
    log_and_print(report_lines, "  [4/4] LIVE RAM HASH & ASSET SCANNER (live_ram_tracer.py)")
    log_and_print(report_lines, "=" * 80 + "\n")
    out = run_script("diagnostics/live_ram_tracer.py", ["--scan-models", "--continuous", "--duration", "30", "--output", "stdout"])
    report_lines.append(out)
    
    # Save the consolidated report
    os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
    report_content = "\n".join(report_lines)
    with open(REPORT_PATH, "w", errors="ignore") as f:
        f.write(report_content + "\n")
        
    # Post-run cleanup: Delete separate files to keep only the single consolidated log
    for redundant_file in ["diagnostics/logs/test_eemem_results.txt", "diagnostics/logs/comparison_report.txt"]:
        if os.path.exists(redundant_file):
            try:
                os.remove(redundant_file)
            except Exception:
                pass
         
    print("\n" + "=" * 80)
    print("  CONSOLIDATED REPORT GENERATION COMPLETE!")
    print(f"  All diagnostic outputs successfully compiled and written to:")
    print(f"  -> {REPORT_PATH}")
    print("  You can now copy the console output above or read the generated file.")
    print("=" * 80)

def main():
    clear_screen()
    print("=" * 80)
    print("    EverQuest Online Adventures  —  Live Diagnostic Suite Shell")
    print("    Targeting: PCSX2 Emotion Engine Main RAM (32MB contiguous)")
    print("=" * 80)
    print("\n[*] Starting complete diagnostics suite and compiling log automatically...")
    print("[*] Note: The live RAM tracer will scan dynamically for exactly 30 seconds.")
    print("=" * 80 + "\n")
    
    run_all_diagnostics()
    
    print("\n" + "=" * 80)
    print("    DIAGNOSTICS COMPLETED SUCCESSFULLY!")
    print("=" * 80)

if __name__ == '__main__':
    main()
