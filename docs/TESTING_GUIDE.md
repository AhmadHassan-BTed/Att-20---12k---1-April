# TESTING GUIDE - EQOA Character Visibility Fix

## Pre-Test Checklist

- [x] Patched ISO created and verified: `iso/patched/EQOA_Frontiers_Patched.iso`
- [x] Pristine Structural Upgrade pipeline (True Hybrid Graft) verified ✓
- [x] Stale PCSX2 savestate deleted ✓
- [x] File structure correctly organized into `docs/`, `core/`, and `diagnostics/` ✓

---

## Step-by-Step Testing Procedure

### Phase 1: Clean Emulator State

1. **Close PCSX2 completely**
   - Make sure there are no background processes

2. **Verify savestate is deleted**
   - Ensure you do not load any old savestates, as they contain the old broken RAM state.

### Phase 2: Load Patched ISO

3. **Launch PCSX2**

4. **Configure ISO:**
   - Go to **CDVD > ISO Selector > Browse**
   - Select: `iso\patched\EQOA_Frontiers_Patched.iso`

5. **Force Cold Boot**
   - Go to **System > Boot ISO (fast)**
   - Do **NOT** select resume from savestate. 

### Phase 3: Verify Character Visibility

6. **Boot Game**
   - Wait for BIOS → Game Splash → Main Menu

7. **Connect to Sandstorm Server**
   - Log in.

8. **Spawn Character**
   - Enter the game world.

9. **EXPECTED RESULT: Character Model is FULLY VISIBLE**
   - Because we used the True Hybrid Graft (Frontiers skeleton + Vanilla mesh), the rendering engine will correctly process the VU1 vector math and display the character flawlessly.

---

## Technical Notes

The updated **Pristine Structural Upgrade (True Hybrid Graft)** works by:
1. **Using** the native Frontiers character model (`0x72700`) as a pristine base to guarantee 100% skeleton (`0x02800`) and bone definition (`0x22400`) compatibility.
2. **Injecting** the Vanilla Mesh Container (`0x02610`) directly into this Frontiers base.
3. **Injecting and Translating** the Vanilla Textures to match GS TEX0 registers.
4. **Rebuilding** the ESF and ISO.

When PCSX2 loads the ISO, the VU1 vertex shader receives exactly the bone array sizes it hardcodedly expects from Frontiers, but draws the mesh vertices from Vanilla!

---

## Using the Master Tool

If you ever need to rebuild the ISO or check the active RAM logs:

Simply double-click **`EQOA_MASTER_TOOL.bat`** in the main folder.
- **Option 1**: Repacks the ISO using the True Hybrid Graft.
- **Option 2**: Runs the Live RAM Diagnostics suite for 30 seconds and outputs to `diagnostics\logs\latest_diagnostic_log.txt` and `diagnostics\logs\history_diagnostic_log.txt`.
