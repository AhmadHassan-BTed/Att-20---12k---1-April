# EQOA Character Transplant - Completion Summary

## The Core Problem: Invisible Character Models
When Vanilla character models were transplanted into the Frontiers client, they became entirely invisible. The game ran fine and parsed the ESF archive correctly, but Vector Unit 1 (VU1) failed to draw the vertices. 

Initial attempts tried to just relabel the Vanilla character root container (`0x62700` -> `0x72700`) and the bone container (`0x12400` -> `0x22400`). However, the Frontiers engine hardcodes an expectation for a 95KB skeleton and a 2.2KB bone definition array. Passing the 14KB Vanilla skeleton caused a complete structural mismatch in bone indices, making the shader collapse the model.

## The Solution: True Hybrid Graft
We implemented a **True Hybrid Graft** in `core/vanilla_to_frontiers_transplant.py`.
Instead of forcing the Vanilla skeleton into the Frontiers engine, we:
1. Load the original Frontiers model as the structural base template.
2. Surgically extract the Vanilla Mesh Container (`0x02610`).
3. Graft the Vanilla Mesh directly into the Frontiers skeleton.
4. Translate and graft the Vanilla Textures to match the GS Registers.

This ensures the 3D graphics pipeline gets the exact skeletal memory allocations it expects, but wraps it around the Vanilla vertices and textures, perfectly restoring visibility!

## Architecture & Workflow Restructuring
- **File Structure**: Organized into `docs/` (documentation), `core/` (python tools), `diagnostics/` (logging/scanners), and `legacy/`.
- **EQOA_MASTER_TOOL.bat**: A single, auto-admin-elevating script to orchestrate everything.
- **Logging**: The diagnostics suite now produces exactly two files: `latest_diagnostic_log.txt` and `history_diagnostic_log.txt`, automatically cleaning up anything else.

## Next Steps for User
1. Run `EQOA_MASTER_TOOL.bat` Option [1] to build the ISO.
2. Cold boot PCSX2 with the new ISO (do NOT load a savestate).
3. Connect and see your perfectly visible character!
