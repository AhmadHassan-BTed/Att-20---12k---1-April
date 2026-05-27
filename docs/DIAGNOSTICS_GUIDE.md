# EQOA Diagnostic Tools Documentation

## Quick Start

**All actions should now be run from the unified master tool in the main directory:**
Double click `EQOA_MASTER_TOOL.bat` and select Option [2].

This will run the Live RAM diagnostics automatically for 30 seconds and generate exactly two log files.

---

## Log Outputs

When Option [2] is run, the suite writes all diagnostic output to two specific files in `diagnostics/logs/`:

1. **`latest_diagnostic_log.txt`**
   - This file is completely overwritten every time you run the tool.
   - It contains *only* the output from your most recent run.
   
2. **`history_diagnostic_log.txt`**
   - This file appends over time. 
   - Each new run is separated by a massive `============= SESSION START: [Date/Time] =============` break, so you can easily scroll through past runs.

*(Note: The suite automatically cleans up any other intermediate loose logs like `consolidated_diagnostics_log.txt` or `live_memory_dump.txt` to keep the folder clean).*

---

## What the Live RAM Diagnostics Suite Does

The automated script (`diagnostics/diagnostic_suite.py`) performs 4 major steps sequentially:

1. **DETECTING EE RAM BASE (`test_eemem_find.py`)**
   - Dynamically hooks into the running `pcsx2-qt.exe` process.
   - Finds the 32MB contiguous Emotion Engine Main RAM block using exported `EEmem` pointers or deep memory scanning.

2. **MODEL TREE STRUCTURAL COMPARATOR (`compare_loaded_roots.py`)**
   - Dumps active memory and traverses loaded ESF containers.
   - Outputs the node tree (children, sizes, offsets) of any character models currently loaded in the game.

3. **ACTIVE RAM TRANSITION DUMPER**
   - Checks state transitions in active models.

4. **LIVE RAM HASH & ASSET SCANNER (`live_ram_tracer.py`)**
   - Scans continuously for 30 seconds looking for the 11 specific Vanilla character hashes (`0x2EF8E480`, `0x05AEBA67`, etc.)
   - If found, it outputs a pristine hex-dump of the surrounding context in memory to see exactly how the engine is handling the pointers.

---

## File Structure

The project has been organized logically:

```
t:\Att 20 - 12k - 1 April\
├── EQOA_MASTER_TOOL.bat            # THE ONLY FILE YOU NEED TO CLICK
├── core/
│   ├── vanilla_to_frontiers_transplant.py # True Hybrid Graft script
│   ├── esf_parser.py
│   └── repack_iso.py
├── diagnostics/
│   ├── diagnostic_suite.py         # Main script run by Option [2]
│   ├── live_ram_tracer.py
│   ├── test_eemem_find.py
│   └── logs/
│       ├── latest_diagnostic_log.txt
│       └── history_diagnostic_log.txt
├── legacy/
│   └── bats/                       # Old individual run_*.bat files
├── docs/                           # Documentation and guides
├── iso/
│   └── patched/EQOA_Frontiers_Patched.iso # The final playable game
└── workspace/
    └── target_assets.json
```

---

## Troubleshooting "Invisible Character"

If you ever see the character invisible again:
1. Verify you used **Option 1** in the Master Tool to patch the game correctly.
2. Verify you booted the game from a **COLD BOOT** (System -> Boot ISO), and did NOT load a savestate. Savestates cache the old broken memory!
3. Run **Option 2** while in-game, and provide `diagnostics/logs/latest_diagnostic_log.txt` to the AI agent for analysis.
