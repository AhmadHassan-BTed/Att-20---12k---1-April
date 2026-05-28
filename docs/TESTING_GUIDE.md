# TESTING GUIDE - EQOA Character Visibility Fix

## Pre-Test Checklist

- [x] Community repositories dynamically linked via `core/link_repos.py`
- [x] Asset IDs aligned with `EQOA-Data` JSON schemas via `core/manifest_aligner.py`
- [x] Geometry mathematically sanitized (NaNs/Infs neutralized via `core/geometry_sanitizer.py`)
- [x] Patched ISO compiled and hash-verified: `iso/patched/EQOA_Frontiers_Patched.iso`
- [x] Stale PCSX2 savestates (`sstates` folder) and cache logs deleted

---

## Step-by-Step Testing Procedure

### Phase 1: Clean Emulator State (Mandatory)

1. **Kill PCSX2 completely**

- Check Task Manager to ensure no background `pcsx2.exe` processes are hanging.

2. **Purge the Cache**

- Ensure you do not load any old savestates. The emulator RAM must be completely sterile to force a fresh read of the new UDF filesystem.

### Phase 2: Load Patched ISO (The "Sterile" Boot)

3. **Launch PCSX2**
4. **Configure ISO:**

- Go to **CDVD > ISO Selector > Browse**
- Select: `iso\patched\EQOA_Frontiers_Patched.iso`

5. **Force a FULL BOOT (CRITICAL STEP):**

- Go to **Settings > System** and ensure **"Enable Fast Boot" is UNCHECKED**.
- Go to **System > Boot CDVD (full)**.
- _Why?_ Fast Boot skips the BIOS and uses cached file system indexes. Full Boot forces the PS2 BIOS to re-initialize the DVD drive and read our modified LBA pointers directly from the disc.

### Phase 3: Verify Character Visibility

6. **Boot Game**

- Wait for BIOS → Game Splash → Main Menu. Check the console log for any `TLB Miss` errors during the loading screen.

7. **Connect to Sandstorm Server**

- Log in.

8. **Spawn Character**

- Enter the game world.

9. **EXPECTED RESULT: Character Model is FULLY VISIBLE**

- Because the geometry math is sanitized and the ESF ID exactly matches the manifest schema, the engine will trigger a cache-hit and send the geometry to the VU1 vector unit successfully.

---

## Technical Notes

The updated **Manifest Alignment & Sanitization Protocol** works by fixing the two silent killers of the render pipeline:

1. **The Math Crash (Fixed):** Vanilla models contained corrupted floats (`NaN`/`Inf`) and out-of-bounds bone indexes. The `geometry_sanitizer.py` cleans this data so the VU1 microprogram doesn't fault.
2. **The Silent Rejection (Fixed):** Even with perfect geometry, if the character's internal ID doesn't match what the server/client manifest expects, the engine skips the render call. By using `eqoa-esf-tools` to parse the `0x72700` node and mapping it against the official `EQOA-Data` JSON schemas, we guarantee the game engine knows _exactly_ where to look for the character asset.

When PCSX2 loads the ISO via Full Boot, the engine queries the manifest, finds our aligned ID, reads the sanitized geometry buffer, and executes the draw call.

---

## Using the Automation Pipeline

If you need to rebuild the ISO or re-align the manifest schemas:

Simply execute **`run_patcher.bat`** in the main folder.

- **Phase 1**: Sanitizes geometry and aligns ESF IDs against community schemas.
- **Phase 2**: Rebuilds the custom ESF database (`FINAL_CHAR_MERGED.ESF`).
- **Phase 3**: Repacks the ISO and outputs strictly to `iso\patched\EQOA_Frontiers_Patched.iso`.
