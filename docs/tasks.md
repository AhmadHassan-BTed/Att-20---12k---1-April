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
- [ ] Implement command line argument mapping
- [ ] Parse target ESF maps dynamically
- [ ] Iterate target assets list
  - [ ] Extract Vanilla `0x62700` payload
  - [ ] Upgrade root type_id to `0x72700`
  - [ ] Upgrade Child 6 bone type_id from `0x12400` to `0x22400`
  - [ ] Append Child 15 (`0x02950`, size 0)
  - [ ] Append Child 16 (`0x02960`, size 4, value `0x00000000`)
  - [ ] Re-calculate node sizes recursively
- [ ] Save output hybrid payloads
- [ ] Trigger ESF database re-compilation
- [ ] Trigger sector repacking
- [ ] Perform dynamic UDF File Entry binary patches

---

## 3. Verification & Emulation Check
- [ ] Run `verify_injected_models.py` and confirm 11 hybrid outputs have identical geometry size to Vanilla
- [ ] Run `verify_final_patch.py` and confirm logical/physical offsets
- [ ] Run `verify_final_iso.py` and confirm byte match
- [ ] Boot game in PCSX2 and verify character visibility in-game on Sandstorm
