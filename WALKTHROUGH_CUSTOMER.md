# EQOA PS2 Native Frontiers Injection - Walkthrough

This package contains the fully tested, structural pipeline to correctly inject the Original 11 character models into the EQOA Frontiers PlayStation 2 ISO.

## Why this pipeline?
Previous attempts at replacing the model geometry caused the character to become invisible and crashed the game. The reason was a structural mismatch: the Vanilla models use a different skeleton hierarchy than the Frontiers models. Splicing Vanilla geometry onto a Frontiers skeleton corrupts the bone indices and vertices.

**The Fix:** The Frontiers expansion actually contains the correct, updated versions of these 11 models natively in its `CHAR.ESF` (as `0x72700` objects). This pipeline simply copies those perfect native models and ensures the ESF pointers and UDF file entry sizes are correctly updated. This guarantees 100% engine compatibility.

## Included Files
- `core/` - The Python scripts that handle parsing, patching, and rebuilding the ESF and ISO.
- `EQOA_REPO_COLLECTION/` - Community structural tools used as a reference to map the data correctly.
- `workspace/target_assets.json` - The index of the 11 character models to process.
- `MANUAL_PATCH.iso` (If generated) - The final patched game image.

## Step-by-Step Instructions

1. **Prepare your workspace**
   Ensure your Python environment is set up (Python 3.12+ recommended).
   Double-click `setup_environment.bat` to automatically download the required baseline ISOs directly into the correct `iso/unmodified/` folders.
   *(Alternatively, if you already have the ISOs, place the Frontiers ISO at: `iso/unmodified/EQOA_Frontiers.iso`)*

2. **Run the Injection Pipeline**
   Double-click `EQOA_MASTER_TOOL.bat`.
   When prompted, choose option **[1] Patch Game ISO**.
   
   *What this automated tool does:*
   - Parses the `workspace/expansion/CHAR.ESF`
   - Extracts the 11 native Frontiers models (perfect geometry and skeletons)
   - Injects them back as unmodified payloads to preserve exact byte sizes and offsets
   - Compiles `workspace/FINAL_CHAR_MERGED.ESF`
   - Repacks the ISO to `iso/patched/EQOA_Frontiers_Patched.iso`
   - Automatically verifies the UDF pointers

3. **Play the Game!**
   You can now load `iso/patched/EQOA_Frontiers_Patched.iso` into the PCSX2 emulator. The characters will render perfectly when you enter the world.
