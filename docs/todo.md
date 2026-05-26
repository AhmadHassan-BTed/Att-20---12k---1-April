# EQOA Character Model Pipeline — High-Level Todo List

This checklist tracks high-level milestones and architectural deliverables required to complete the model restoration project successfully.

---

## High-Level Backlog & Milestones

- [ ] **Phase 1: Engineering Infrastructure**
  - [x] Create comprehensive engineering tracking tracking files (`architecture.md`, `tech_stack.md`, `progress.md`, `tasks.md`, `todo.md`)
  - [x] Establish a clean workspace and delete redundant, giant files

- [ ] **Phase 2: Pristine Structural Upgrade Pipeline**
  - [ ] Implement `vanilla_to_frontiers_transplant.py` script
  - [ ] Extract the 11 target character models from Vanilla `CHAR.ESF`
  - [ ] Transform extracted models structurally:
    - [ ] Root node type: `0x62700` -> `0x72700`
    - [ ] Bone node type: `0x12400` -> `0x22400`
    - [ ] Expand child list (15 -> 17) and append Child 15 (`0x02950`) and Child 16 (`0x02960`)
  - [ ] Recursively update hierarchical node sizes and rebuild node buffers
  - [ ] Save hybrid model `.bin` payloads

- [ ] **Phase 3: Database Merging & ISO Repacking**
  - [ ] Run `esf_rebuilder.py` to merge the 11 upgraded `.bin` payloads into frontiers template
  - [ ] Run `repack_iso.py` to append the merged `CHAR.ESF` and patch ISO 9660 LBA/sizes
  - [ ] Run `patch_udf_char_esf_v2.py` with dynamic logical LBA calculation (`1,492,090`) and tag checksum computation

- [ ] **Phase 4: Robust Verification & Emulation Validation**
  - [ ] Validate ESF database structure has 0 parsing anomalies (using `verify_injected_models.py`)
  - [ ] Validate ISO file descriptors and binary sector matches (using `verify_final_patch.py` and `verify_final_iso.py`)
  - [ ] Close emulator and verify character model visibility in-game
