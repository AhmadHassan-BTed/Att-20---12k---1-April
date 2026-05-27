# EQOA Diagnostic Tools Documentation

## Quick Start

**Run all diagnostics in one command:**
```bash
python run_all_diagnostics.py
```

Or on Windows:
```cmd
run_diagnostics.bat
```

This executes 4 comprehensive diagnostic checks and provides a unified report.

---

## Diagnostic Tools Overview

### 1. **Integrated Diagnostics** (`integrated_diagnostics.py`)

**Purpose**: Master diagnostic that performs complete ISO, ESF, and PCSX2 verification in a single run.

**What it checks**:
- ✓ Patched ISO integrity (file size, LBA location)
- ✓ Unmodified ISO comparison baseline
- ✓ Patched ESF asset verification (all 11 target models)
- ✓ Vanilla character model sizes (reference)
- ✓ Frontiers character model sizes (comparison)
- ✓ ISO file structure (all ESF files listed)
- ✓ Duplicate ESF detection (FJBO signature count)
- ✓ PCSX2 savestate status
- ✓ PCSX2 emulation log analysis

**Run**:
```bash
PYTHONPATH=. python integrated_diagnostics.py
```

**Output**: Comprehensive report with [OK]/[WARNING]/[ERROR] status for each section

**Expected Results** (Success State):
```
Patched ISO: 148,838,890 bytes [OK]
Patched Assets: 11/11 found [OK]
FJBO Signatures: 11 (expected) [OK]
Savestates: None found [OK]
Recommendation: ISO is ready for testing in PCSX2
```

---

### 2. **ISO Patch Verification** (`check_iso_appended_bytes.py`)

**Purpose**: Confirm that patched CHAR.ESF data was actually written to the correct location in the ISO.

**What it checks**:
- Reads first 32 bytes of `workspace/FINAL_CHAR_MERGED.ESF` (expected patch)
- Reads bytes from ISO at LBA 1492368 (where CHAR.ESF is located)
- Compares hex headers to verify they match

**Run**:
```bash
PYTHONPATH=. python workspace/scratch/check_iso_appended_bytes.py
```

**Output**: Hex comparison and [OK]/[ERROR] status

**Expected Result**:
```
Expected header: 464a424f0100000...
Actual header:   464a424f0100000...
[OK] Headers MATCH - patch was written correctly!
```

**When to use**: If you suspect the patch wasn't written to the ISO or was corrupted.

---

### 3. **ESF Asset Comparison** (`compare_esf_sizes.py`)

**Purpose**: Analyze model size transformations across all three ESF databases to understand the upgrade.

**What it checks**:
- Lists Vanilla CHAR.ESF (410 assets, 108.1 MB)
- Lists Frontiers CHAR.ESF (582 assets, 148.3 MB)
- Lists Patched CHAR.ESF (582 assets, 148.8 MB)
- Compares sizes of all 11 target models
- Identifies asset status: VANILLA, FRONTIERS, HYBRID, or MISSING

**Run**:
```bash
PYTHONPATH=. python workspace/scratch/compare_esf_sizes.py
```

**Output**: Table showing size for each model in each database

**Expected Results**:
```
Hash       Vanilla    Frontiers   Patched    Status
0x2EF8E480  552,763     506,351    552,791    HYBRID
0x05AEBA67  551,160     495,696    551,188    HYBRID
...all 11 models should show HYBRID status...
```

**Interpretation**:
- **HYBRID** = Asset is Vanilla model upgraded to Frontiers format (CORRECT)
- **VANILLA** = Asset is unmodified Vanilla (only if intentional)
- **FRONTIERS** = Asset is native Frontiers (only if intentional)
- **MISSING** = Asset not found (ERROR - contact support)

**When to use**: If you need to verify the structural upgrade worked correctly.

---

### 4. **PCSX2 File Access Check** (`check_pcsx2_open_files.py`)

**Purpose**: Determine which ISO files PCSX2 can access and whether it has any files locked.

**What it checks**:
- Finds running PCSX2 process(es)
- Lists open handles/files for each PCSX2 instance
- Tests accessibility of all three ISO files:
  - `iso/patched/EQOA_Frontiers_Patched.iso`
  - `iso/unmodified/EQOA_Frontiers.iso`
  - `iso/unmodified/EQOA_Original.iso`

**Run**:
```bash
PYTHONPATH=. python workspace/scratch/check_pcsx2_open_files.py
```

**Output**: PCSX2 process info and ISO file accessibility status

**Expected Result** (When PCSX2 is running with patched ISO loaded):
```
Process: pcsx2-qt (PID: 20912)
iso/patched/EQOA_Frontiers_Patched.iso: 3.2 GB [ACCESSIBLE]
iso/unmodified/EQOA_Frontiers.iso: 3.0 GB [ACCESSIBLE]
```

**Expected Result** (When PCSX2 is NOT running):
```
[INFO] PCSX2 is not currently running
       Cannot check open files
```

**When to use**: 
- If you need to verify which ISO PCSX2 loaded
- If character is still invisible and you want to confirm the right ISO is loaded
- If you get file access errors

---

### 5. **Master Diagnostic Runner** (`run_all_diagnostics.py`)

**Purpose**: Orchestrates all diagnostic scripts in sequence and provides unified summary.

**What it does**:
1. Runs `integrated_diagnostics.py`
2. Runs `check_iso_appended_bytes.py`
3. Runs `compare_esf_sizes.py`
4. Runs `check_pcsx2_open_files.py`
5. Collects all results
6. Prints unified summary with pass/fail for each

**Run**:
```bash
PYTHONPATH=. python run_all_diagnostics.py
```

Or simply:
```cmd
run_diagnostics.bat
```

**Output**: 
- Individual diagnostic outputs from each script
- Final summary table
- Actionable recommendations

**Expected Result**:
```
[OK] Integrated Diagnostics
[OK] ISO Patch Verification
[OK] ESF Asset Comparison
[OK] PCSX2 File Access

[FINAL RESULT] All diagnostics passed successfully

RECOMMENDATION:
  1. Close PCSX2 completely
  2. Relaunch PCSX2 fresh (no savestate resume)
  3. Load: iso/patched/EQOA_Frontiers_Patched.iso
  4. Connect to Sandstorm server and spawn a character

Character should now be FULLY VISIBLE
```

---

## Troubleshooting Guide

### Scenario 1: Character is Still Invisible

**Step 1**: Run diagnostics
```bash
python run_all_diagnostics.py
```

**Check for**:
- All diagnostics show [OK]
- Patched ISO size is 148,838,890 bytes
- All 11 models found in patched database
- No savestates in PCSX2 folder

**If diagnostics all pass but character still invisible**:
1. Close PCSX2 completely: `taskkill /IM pcsx2-qt.exe /F`
2. Verify no remaining PCSX2 processes: `Get-Process pcsx2*`
3. Relaunch PCSX2 from scratch
4. Load ISO from scratch (don't resume)
5. Test again

---

### Scenario 2: Patched ISO Size is Wrong

**Step 1**: Check ISO
```bash
get-item iso/patched/EQOA_Frontiers_Patched.iso | select Length
```

**Expected**: 3,205,212,160 bytes (reports as 148,838,890 bytes in CHAR.ESF)

**If different**:
- Patch may have failed during ISO repack
- Run `python vanilla_to_frontiers_transplant.py` to regenerate
- Verify patch completed without errors

---

### Scenario 3: Assets Missing or Corrupted

**Step 1**: Check ESF comparison
```bash
PYTHONPATH=. python workspace/scratch/compare_esf_sizes.py
```

**If models show MISSING or FRONTIERS status** (not HYBRID):
- ESF database may be corrupted
- Regenerate merged ESF: `python workspace/FINAL_CHAR_MERGED.ESF`
- Repack ISO with updated ESF
- Run diagnostics again

---

### Scenario 4: "Headers Don't Match" in Patch Verification

**Step 1**: Check patch write
```bash
PYTHONPATH=. python workspace/scratch/check_iso_appended_bytes.py
```

**If headers don't match**:
- Patch was not written to ISO correctly
- ISO may be read-only or corrupted
- Verify disk space available
- Verify file permissions are writable
- Delete old patched ISO and regenerate

---

## Common Issues & Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| All checks pass but character invisible | Old savestate being resumed | Delete PCSX2 savestate, cold boot fresh |
| "Patched ISO size wrong" | Repack incomplete/failed | Regenerate ISO with `vanilla_to_frontiers_transplant.py` |
| "Headers don't match" | Patch write failed | Verify disk space, permissions, regenerate ISO |
| "Assets show FRONTIERS not HYBRID" | ESF merge incomplete | Regenerate merged ESF database |
| "FJBO signatures > 11" | Corrupted/duplicate ESF copies | Delete patched ISO, regenerate clean |
| "PCSX2 can't open ISO" | File permissions, disk error | Check file exists, permissions readable, no virus lock |

---

## Diagnostic Output Interpretation

### Color-coded Status Messages

| Status | Meaning |
|--------|---------|
| `[OK]` | Check passed, system is healthy |
| `[WARNING]` | Non-critical issue detected, may affect functionality |
| `[ERROR]` | Critical issue, requires action |
| `[SKIP]` | Check not applicable (file not found, process not running) |
| `[HYBRID]` | Asset is Vanilla model in Frontiers format (expected) |
| `[VANILLA]` | Asset is unmodified original (expected for non-patched assets) |
| `[FRONTIERS]` | Asset is native Frontiers format (expected for Frontiers-only models) |
| `[MISSING]` | Asset not found in database (error) |
| `[LOCKED]` | File cannot be opened by other processes (info) |
| `[ACCESSIBLE]` | File can be read by PCSX2 (good sign) |

---

## For AI Agents & Automated Debugging

### Single Command Execution
```bash
python run_all_diagnostics.py
```

### Pros for Automated Systems
- ✅ Single command, no loops needed
- ✅ Comprehensive output in one execution
- ✅ Pass/fail status for each diagnostic
- ✅ Clear recommendations provided
- ✅ No need to interpret multiple separate outputs
- ✅ Reduced API calls and context usage

### Integration Example
```python
result = subprocess.run(
    ['python', 'run_all_diagnostics.py'],
    capture_output=True,
    timeout=60
)

if result.returncode == 0:
    # Parse output for status
    # Make recommendations
else:
    # Handle diagnostic failures
```

---

## File Structure

```
t:\Att 20 - 12k - 1 April\
├── integrated_diagnostics.py          # Master unified diagnostic
├── run_all_diagnostics.py             # Orchestrator script
├── run_diagnostics.bat                # Windows convenience wrapper
├── workspace/scratch/
│   ├── check_iso_appended_bytes.py    # Patch write verification
│   ├── check_pcsx2_open_files.py      # PCSX2 file access check
│   ├── compare_esf_sizes.py           # Asset size comparison
│   ├── final_verification.py          # Simple ISO verification
│   └── [other diagnostic tools]
├── iso/
│   ├── patched/EQOA_Frontiers_Patched.iso
│   └── unmodified/
│       ├── EQOA_Frontiers.iso
│       └── EQOA_Original.iso
└── workspace/
    ├── original/CHAR.ESF             # Vanilla database
    ├── expansion/CHAR.ESF            # Frontiers database
    ├── FINAL_CHAR_MERGED.ESF         # Patched/merged database
    └── target_assets.json            # 11 character model list
```

---

## Next Steps

1. **Run diagnostics**: `python run_all_diagnostics.py`
2. **Review output** for any [WARNING] or [ERROR] messages
3. **Follow recommendations** provided in summary
4. **Test in PCSX2**: Load game and spawn character
5. **Verify visibility**: Character should now be fully visible

**Status**: All diagnostic tools verified and integrated ✓
