# EQOA Character Model Pipeline — Progress & Execution Log

This document records the chronological history of test runs, breakthroughs, issues identified, and resolved diagnostics.

---

## 1. Chronological Log & Execution History

### 2026-05-26 19:30 (Initial Diagnostic Run)
* **Status**: Re-started pairing task with the user.
* **Findings**:
  - The unmodified Frontiers ISO shows characters normally in-game on the Sandstorm server.
  - The patched ISO with the texture-only grafted hybrid models resulted in invisible characters.
  - Mesh structures parsed cleanly (`0x72700`, 17 children), but geometry was mismatched with skeleton weights (causing silent render collapses on vertex shader drawing).
* **Action Taken**: Formulated the Pristine Structural Upgrade theory to upgrade the complete Vanilla model including geometry and bones rigging into a native `0x72700` wrapper.

### 2026-05-26 19:45 (UDF Descriptor Investigation)
* **Breakthrough**:
  - Found that the UDF File Entry patcher was failing to apply the new LBA pointer because it was searching for the physical address `3578` instead of the logical address `3300` (which is logical offset under UDF partition offset 278).
  - Also corrected logical sector writing: it was attempting to write `1,492,368` instead of the logical sector `1,492,090`.
* **Action Taken**: Corrected UDF logical mapping formulas in `patch_udf_char_esf_v2.py`. Re-patched sectors.
* **Result**: `[PASS]` All verification checks on `verify_final_patch.py` and `verify_final_iso.py` passed 100% with exact sector-level binary matching!

---

## 2. Test Run Results Table

| Date/Time | Pipeline Phase | Test Executed | Target | Result Status | Notes |
|-----------|----------------|---------------|--------|---------------|-------|
| 05-26 19:32 | Surgery | `clean_surgery_pipeline` | payloads bin | `SUCCESS` | Mesh sizes matched Vanilla, wrappers updated. |
| 05-26 19:32 | Database | `esf_rebuilder` | CHAR.ESF | `SUCCESS` | Delta offset of `+694,956` bytes applied cleanly. |
| 05-26 19:32 | ISO Repack | `repack_iso` | ISO9660 | `SUCCESS` | Appended ESF at physical LBA `1,492,368`. |
| 05-26 19:33 | UDF Patch | `patch_udf_char_esf_v2` | Sector 337 | `SUCCESS` | Patched size to `149,065,928` & logical LBA to `1,492,090`. |
| 05-26 19:33 | Verify | `verify_final_patch` | ISO descriptors | `PASS` | All descriptor fields matched expected dynamically. |
| 05-26 19:33 | Verify | `verify_final_iso` | disc Sector | `PASS` | Sector byte comparison is 100% identical. |
