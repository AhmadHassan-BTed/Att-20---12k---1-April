# Implementation Plan — ISO Storage Reorganization & Pipeline Alignment

This plan defines the comprehensive path reorganization and code modification tasks to segregate unmodified base ISO files from custom generated patched builds. 

It also incorporates a dedicated technical analysis of the **invisible character rendering bug** currently under diagnosis, detailing where it occurs and the engineering approach to resolve it.

---

## 1. Technical Problem Description (Invisible Character Symptom)

### The Symptom
When booting the patched ISO on the PCSX2 emulator and connecting to the Sandstorm server:
* The character model parses without syntax crashes.
* However, **once the player spawns into the game world**, the character model is **completely invisible** on screen (does not show).
* **Contrast:** In the unmodified Frontiers expansion ISO, character models display and render perfectly under identical emulator settings and server connections.

### Low-Level Diagnostic Analysis
Prior contractor attempts utilized a "texture-only graft" approach. While this successfully injected the Vanilla texture files into the Frontiers database structural templates, it created a severe mismatch:
1. **Bone Rigging Mismatch**: The Frontiers bone rigging nodes (`0x02800` / `0x22400` container structures) did not align with the Vanilla mesh vertex layout data (`0x02610` container).
2. **VU1 Shader Collapse**: Because the bone weights, joint matrices, and vertex vertices were out of sync, the PlayStation 2 Vector Unit 1 (VU1) vertex shader collapsed the geometry vertices to `(0, 0, 0)` at draw time.
3. **Result**: The game engine attempts to draw the model, but it renders as an infinitely small/collapsed point, rendering the character completely invisible without crashing the emulator.

### Resolution Strategy (Pristine Structural Upgrade)
Instead of splicing textures into Frontiers assets, our pipeline executes a **complete structural upgrade** of the entire Vanilla model (incorporating Vanilla's geometry, bones rig, and textures in 100% unison) into a Frontiers-compliant `0x72700` wrapper container. This ensures that the VU1 vector unit receives a fully matched set of meshes and bones, restoring in-game visibility!

---

## 2. Complete Project Directory Architecture (Target State)

```
t:\Att 20 - 12k - 1 April\
├── docs/                           # Layer 1: Systematic Record Keeping & Docs
│   ├── architecture.md             # Low-level FJBO, disk formatting, and folder map [TO UPDATE]
│   ├── tech_stack.md               # Technology reference catalog and path index [TO UPDATE]
│   ├── todo.md                     # High-level backlogs [TO UPDATE]
│   ├── progress.md                 # Live progress and execution log [TO UPDATE]
│   ├── tasks.md                    # Component-level task board [TO UPDATE]
│   └── implementation_plan.md      # This file! [NEW]
├── core/                           # Layer 2: Core Engineering & Pipeline Logic
│   ├── esf_parser.py               # FJBO recursive parsing engine [ACTIVE]
│   ├── esf_rebuilder.py             # ESF model container database rebuilder [ACTIVE]
│   ├── repack_iso.py               # ISO repacking and sector aligner [ACTIVE - TO MODIFY]
│   ├── patch_udf_char_esf_v2.py    # Surgical logical LBA and size patcher [ACTIVE - TO MODIFY]
│   ├── verify_final_patch.py       # PVD and File Entry validator [ACTIVE - TO MODIFY]
│   ├── verify_final_iso.py         # Sector-level binary validator [ACTIVE - TO MODIFY]
│   ├── verify_injected_models.py   # Tree structure and size validator [ACTIVE]
│   └── payload_extractor.py        # Complete classic model payload extractor [UTILITY]
├── workspace/                      # Layer 5: Target Resources & Assets
│   ├── target_assets.json          # Configuration list for the 11 character hashes [ACTIVE]
│   ├── original/                   # Original Vanilla CHAR.ESF database [ACTIVE]
│   ├── expansion/                  # Native Frontiers CHAR.ESF database [ACTIVE]
│   ├── payloads/                   # Extracted and upgraded hybrid .bin payloads [ACTIVE]
│   └── FINAL_CHAR_MERGED.ESF       # Rebuilt and injected custom ESF database [ACTIVE]
├── iso/                            # Layer 6: Source & Patched ISO Storage (Structured)
│   ├── unmodified/                 # Unmodified base/original input ISOs [NEW]
│   │   ├── EQOA_Original.iso       # Unmodified original base ISO
│   │   ├── EQOA_Frontiers.iso      # Unmodified frontiers expansion base ISO
│   │   └── EQOA_Backup.iso         # Base backup copy
│   └── patched/                    # Compiled and sector-aligned output ISOs [NEW]
│       └── EQOA_Frontiers_Patched.iso # The final generated patched ISO
├── vanilla_to_frontiers_transplant.py # Master Pristine Structural Transplant Pipeline [ACTIVE]
└── run_patcher.bat                 # Master automation wrapper [ACTIVE - TO MODIFY]
```

---

## 3. Proposed Changes by Component

### 3.1 File System Relocations
- Create directory `iso/unmodified/` and move:
  - `iso/EQOA_Backup.iso` -> `iso/unmodified/EQOA_Backup.iso`
  - `iso/EQOA_Frontiers.iso` -> `iso/unmodified/EQOA_Frontiers.iso`
  - `iso/EQOA_Original.iso` -> `iso/unmodified/EQOA_Original.iso`
- Create directory `iso/patched/` and prepare for patched generation.
- Delete the old redundant `output/` directory once empty.

---

### 3.2 Core Python Pipelines & Batch Scripts

#### [MODIFY] [repack_iso.py](file:///t:/Att%2020%20-%2012k%20-%201%20April/core/repack_iso.py)
Change file search and target paths to use the new structured directories:
```python
    iso_path = 'iso/unmodified/EQOA_Frontiers.iso'
    patched_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
```

#### [MODIFY] [patch_udf_char_esf_v2.py](file:///t:/Att%2020%20-%2012k%20-%201%20April/core/patch_udf_char_esf_v2.py)
Update target ISO output path:
```python
ISO_PATH  = 'iso/patched/EQOA_Frontiers_Patched.iso'
```

#### [MODIFY] [verify_final_iso.py](file:///t:/Att%2020%20-%2012k%20-%201%20April/core/verify_final_iso.py)
Update direct byte inspection target:
```python
iso_path = 'iso/patched/EQOA_Frontiers_Patched.iso'
```

#### [MODIFY] [verify_final_patch.py](file:///t:/Att%2020%20-%2012k%20-%201%20April/core/verify_final_patch.py)
Update UDF File Entry allocation descriptor target:
```python
with open('iso/patched/EQOA_Frontiers_Patched.iso', 'rb') as f:
```

#### [MODIFY] [run_patcher.bat](file:///t:/Att%2020%20-%2012k%20-%201%20April/run_patcher.bat)
Update final success message print to guide the user to the correct folder:
```batch
echo Your new game file is: iso/patched/EQOA_Frontiers_Patched.iso
```

---

### 3.3 Engineering Documentation Updates (`.md` Files)

#### [MODIFY] [architecture.md](file:///t:/Att%2020%20-%2012k%20-%201%20April/docs/architecture.md)
Update the filesystem visualization tree and reference list to document `iso/unmodified/` and `iso/patched/` directories instead of old generic `iso/` and `output/`.

#### [MODIFY] [tech_stack.md](file:///t:/Att%2020%20-%2012k%20-%201%20April/docs/tech_stack.md)
Update all operational folder references to specify the new isolated subdirectories.

#### [MODIFY] [progress.md](file:///t:/Att%2020%20-%2012k%20-%201%20April/docs/progress.md)
Add a log entry documenting the structural reorganization of the ISO files.

#### [MODIFY] [todo.md](file:///t:/Att%2020%20-%2012k%20-%201%20April/docs/todo.md)
Add a tracking checkbox under a new "Phase 5: Repository Organization & Cleanliness" section.

#### [MODIFY] [tasks.md](file:///t:/Att%2020%20-%2012k%20-%201%20April/docs/tasks.md)
Add the granular file moving and path editing tasks to the component task board.

---

## 4. Verification Plan

### Automated Pipeline Verification
1. Relocate files and apply code edits.
2. Execute `run_patcher.bat` which launches the full pipeline.
3. Confirm that the pipeline completes successfully and all three verifications (`verify_injected_models.py`, `verify_final_patch.py`, `verify_final_iso.py`) return complete `[PASS]` statuses on the new paths.

### Manual Verification
1. Visually check the directory tree to ensure that original ISO files only reside in `iso/unmodified/`, the patched ISO only resides in `iso/patched/`, and the `output/` folder is removed.
2. Direct PCSX2 to load the ISO from `iso/patched/EQOA_Frontiers_Patched.iso` and verify perfect character model visibility.
