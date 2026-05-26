# EQOA Character Model Pipeline — High-Level Todo List

This checklist tracks high-level milestones and architectural deliverables required to complete the model restoration project successfully.

---

## High-Level Backlog & Milestones

- [x] **Phase 1: Engineering Infrastructure**
  - [x] Create comprehensive engineering tracking tracking files (`architecture.md`, `tech_stack.md`, `progress.md`, `tasks.md`, `todo.md`)
  - [x] Establish a clean workspace and delete redundant, giant files

- [x] **Phase 2: Pristine Structural Upgrade Pipeline**
  - [x] Implement `vanilla_to_frontiers_transplant.py` script
  - [x] Extract the 11 target character models from Vanilla `CHAR.ESF`
  - [x] Transform extracted models structurally:
    - [x] Root node type: `0x62700` -> `0x72700`
    - [x] Bone node type: `0x12400` -> `0x22400`
    - [x] Expand child list (15 -> 17) and append Child 15 (`0x02950`) and Child 16 (`0x02960`)
  - [x] Recursively update hierarchical node sizes and rebuild node buffers
  - [x] Save hybrid model `.bin` payloads

- [x] **Phase 3: Database Merging & ISO Repacking**
  - [x] Run `esf_rebuilder.py` to merge the 11 upgraded `.bin` payloads into frontiers template
  - [x] Run `repack_iso.py` to append the merged `CHAR.ESF` and patch ISO 9660 LBA/sizes
  - [x] Run `patch_udf_char_esf_v2.py` with dynamic logical LBA calculation (`1,492,090`) and tag checksum computation

- [x] **Phase 4: Robust Verification & Emulation Validation**
  - [x] Validate ESF database structure has 0 parsing anomalies (using `verify_injected_models.py`)
  - [x] Validate ISO file descriptors and binary sector matches (using `verify_final_patch.py` and `verify_final_iso.py`)
  - [x] Close emulator and verify character model visibility in-game
