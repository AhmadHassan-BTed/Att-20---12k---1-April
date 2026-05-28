# EQOA Diagnostic Tools & Hardware Trace Documentation

This document chronicles the diagnostic methodologies and low-level discoveries made during the EQOA Vanilla-to-Frontiers Character Restoration Project.

## The `0x0` TLB Miss: A Case Study in EE Buffer Overflows

During development, injecting Vanilla character models into the Frontiers engine resulted in a catastrophic emulator crash:
```
[   12.8102] TLB Miss, pc=0x80005558 addr=0x0 [load]
[   12.8106] TLB Miss, pc=0x80005558 addr=0x0 [load]
[   12.8109] TLB Miss, pc=0x0 addr=0x0 [load]
```

### Initial Hypothesis: The Dirty Float Theory
Initially, it was believed the Vanilla geometry contained "dirty math" (NaN floats, or out-of-bounds bone indices > 42). A `geometry_sanitizer.py` script was written to clamp these values. However, we discovered that in PS2 `V4-32` (`0x6C`) vertex structures, the `W` float component actually contains the Anti-Double-Clipping (ADC) bit in the sign bit. Clamping or zeroing out the `W` component destroyed the ADC flags, entirely corrupting the geometry stream. 

### The Real Cause: EE Skeleton Buffer Overflow
The true cause of the crash was a memory overflow on the Emotion Engine (EE).
The Vanilla Ogre uses a 32-bone skeleton (`0x0B070`). The Frontiers Ogre uses a 43-bone skeleton. The Frontiers Animation Controller running on the EE is hard-coded to generate 43 matrices. 

By injecting the 32-bone Vanilla skeleton, we shrank the allocated array buffer in RAM. The animation controller dutifully calculated 43 bone matrices and wrote them to memory, overflowing the 32-bone array bounds and overwriting adjacent memory—which happened to be the `CharacterModel` object's virtual function table. When the engine called `Render()`, it fetched a corrupted `NULL` pointer and caused a `TLB Miss` at `pc=0x0`.

**The Fix:** Retain the Frontiers `0x0B070` skeleton and `0x02800` bone matrices to preserve the EE buffer sizing, and *only* inject the Vanilla `0x02610` Geometry Node.

## The CDVDman Black Screen

When attempting to build the ISO from scratch using modern Python libraries like `pycdlib` or standard `mkisofs` routines, the game would immediately hang at boot (Black Screen):
```
[  281.4022] ELF Loading: cdrom0:\SLUS_207.44;1, Game CRC = D7321161, EntryPoint = 0x00100008
[  281.4418] CDRead: Reading Sector 0000016 (001 Blocks of Size 2048)
[  281.4584] CDRead: Reading Sector 0000257 (001 Blocks of Size 2048)
```
The PS2's `cdvdman` IOP driver enforces extreme rigidity on ISO9660 / UDF 1.02 directory records and Logical Block Address (LBA) sorting (specifically regarding `SYSTEM.CNF` placement). Generating an ISO using standard interchange levels completely breaks the disc layout.

**The Fix:** Use `core/repack_iso.py` to copy the original, unmodified PS2 ISO, and byte-patch the new payload directly into the binary sector layout (In-Place Repacking). Then, update the UDF Allocation Descriptors (`core/patch_udf_char_esf_v2.py`) to point to the new lengths.

## Diagnostic Scripts Arsenal

While the bug is resolved, the diagnostic scripts written during the autopsy remain available in the `core/` and `workspace/scratch/` directories for future research:

1. **`core/validate_dma.py`**: A parser that mathematically verifies the 16-byte alignment and QWC tags of a PS2 DMA chain to ensure the VU1 microprogram will not crash on load.
2. **`core/audit_materials.py`**: Compares the `0x11110` material/texture dimensions and counts between Vanilla and Frontiers ESFs to diagnose state-dependent rendering rejections.
3. **`core/w_component_audit.py`**: A hexadecimal analysis tool to decode the specific bitflags hidden inside the PS2 float vectors (such as ADC bits).
4. **`core/vif_autopsy.py`**: A hex-dumper for comparing specific VIF descriptors between known-good (Golden Master) payloads and injected payloads.
