# EQOA Character Model Pipeline — Technical Stack Reference

This document catalogs the tools, libraries, file formats, and execution platforms used in the binary grafting and rebuilding process.

---

## 1. Development Tools & Libraries

- **Language**: Python 3.8+ (Strict standard library execution where possible to minimize runtime dependencies).
- **Standard Libraries**:
  - `struct`: Packing and unpacking binary primitives (integers, floats, bytes) in little-endian format.
  - `os`/`sys`: Dynamic paths, subprocess execution, file sizing, and low-level File Descriptor management.
  - `mmap`: Direct OS-level memory mapping of ISO image files to facilitate fast, in-place binary search and patching operations.
  - `shutil`: Robust file copying, backup management, and directory cleaning.
  - `glob`: Directory patterns expansion for tracking batch payloads.
- **Third-Party Libraries**:
  - `construct`: Declarative binary parser and serializer, used for declaring schemas for the `FJBO` ESF headers and node layouts.

---

## 2. Emulation & Networking Environment

- **Target Emulator**: PCSX2 Emulator (v2.6.3+ recommended).
  - *Fast Boot*: Disabled (`EnableFastBoot = false` in `PCSX2.ini` or skipped dynamically) to ensure DNAS bypass and correct sector-aligned loading by the Emotion Engine BIOS.
  - *Console Logs*: Console logging enabled to capture SMAP (DEV9) network activity and VU1 microprogram program caching.
- **Game ID**: SLUS-20744 (EverQuest Online Adventures: Frontiers).
- **Target Server**: Sandstorm Emulator private server.
  - Interacts via the emulated DEV9 network plugin, streaming character spawn packets.
- **disc Sector Layout**: 2048 bytes per sector.

---

## 3. Operations Infrastructure

- **Operating System**: Windows 11 (PowerShell environment).
- **UDF Format**: ECMA-167 compliance.
  - Partition map offset: `278` sectors.
  - UDF descriptor tag identifier: `0x0105` (File Entry).
  - Information Length offset: `0x38` (8 bytes, little-endian).
  - Logical Blocks Recorded offset: `0x40` (8 bytes, little-endian).
  - UDF Tag Checksum offset: `0x04` (1 byte, checksum calculation sum mod 256).
- **ISO 9660**: Primary Volume Descriptor (PVD) located at sector `16` (offset `0x8000`), updating Volume Space Size field.
