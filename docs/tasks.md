# EQOA Character Model Pipeline — Component Task Board

This board registers individual granular actions, code edits, and verification checks.

---

## 1. Engineering Infrastructure
- [x] Create system architecture overview document (`architecture.md`)
- [x] Create project technical stack index (`tech_stack.md`)
- [x] Create backlog checklist tracking file (`todo.md`)
- [x] Create live progress logger document (`progress.md`)
- [x] Create granular task board (`tasks.md`)

---

## 2. Structural Transplant Pipeline (`vanilla_to_frontiers_transplant.py`)
- [x] Implement command line argument mapping
- [x] Parse target ESF maps dynamically
- [x] Iterate target assets list
  - [x] Extract Vanilla `0x62700` payload
  - [x] Upgrade root type_id to `0x72700`
  - [x] Upgrade Child 6 bone type_id from `0x12400` to `0x22400`
  - [x] Append Child 15 (`0x02950`, size 0)
  - [x] Append Child 16 (`0x02960`, size 4, value `0x00000000`)
  - [x] Re-calculate node sizes recursively
- [x] Save output hybrid payloads
- [x] Trigger ESF database re-compilation
- [x] Trigger sector repacking
- [x] Perform dynamic UDF File Entry binary patches

---

## 3. Verification & Emulation Check
- [x] Run `verify_injected_models.py` and confirm 11 hybrid outputs have identical geometry size to Vanilla
- [x] Run `verify_final_patch.py` and confirm logical/physical offsets
- [x] Run `verify_final_iso.py` and confirm byte match
- [x] Boot game in PCSX2 and verify character visibility in-game on Sandstorm
