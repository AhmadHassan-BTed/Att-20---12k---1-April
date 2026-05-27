# EQOA Character Restoration & Hybrid Graft Pipeline

![Platform](https://img.shields.io/badge/Platform-PS2%20(PCSX2)-blue)
![Language](https://img.shields.io/badge/Language-Python%203.12-yellow)
![License](https://img.shields.io/badge/License-MIT-green)

A highly specialized reverse-engineering pipeline and diagnostic suite designed to structurally transplant original Vanilla character models into the *EverQuest Online Adventures: Frontiers* (PS2) game engine, resolving complex Vector Unit 1 (VU1) shader collapse bugs.

## 🚧 Current Status & Call for Contributions

**Current Focus: The Invisible Character Bug**  
At this stage in development, our primary challenge is that **transplanted characters are not appearing/rendering in the game world**. While the assets successfully load into the PlayStation 2's active memory without crashing the emulator, the 3D models remain invisible. 

The current work being done in this repository is entirely focused on solving this rendering collapse. We are actively developing the "True Hybrid Graft" methodology to align the skeletal structures and VRAM texture registers.

**We are completely open to contributions!** If you have experience with PS2 reverse-engineering, VU1 vector math, or the Graphics Synthesizer (GS) pipeline, we welcome your pull requests, insights, and diagnostic help.

## 🧠 The Context & Technical Problem

When attempting to inject original EQOA Vanilla character models (contained in `.ESF` archives) into the updated *Frontiers* client, the character models successfully parsed into memory but rendered completely **invisible**.

Low-level memory tracing revealed the root cause: **A Skeletal Rigging and GS Register Mismatch**.
1. **Container Mismatch**: Vanilla models use a `0x62700` root container (15 children), while Frontiers rigidly expects a `0x72700` container (17 children) with specific metadata trailer nodes.
2. **VU1 Shader Collapse**: Even if node headers were spoofed, the PS2 Vector Unit 1 (VU1) vertex shader relies on hardcoded offsets for the 95KB Frontiers skeleton and 2.2KB bone arrays. Passing the smaller 14KB Vanilla skeleton caused the vertex transformations to multiply by incorrect matrices, collapsing all character geometry to `(0, 0, 0)` at draw time.
3. **Texture Register Rejection**: The PS2 Graphics Synthesizer (GS) rejected the Vanilla VRAM `TEX0` mapping registers, further blocking display.

## 🔬 The Solution: Pristine Structural Transplant (True Hybrid Graft)

Instead of forcing a legacy Vanilla skeleton into a modern engine, this repository implements a **True Hybrid Graft** via Python:

1. **Template Extraction**: Parses the native Frontiers `0x72700` model to serve as a structurally pristine base, guaranteeing 100% bone and skeleton compatibility with the rendering engine.
2. **Surgical Mesh Injection**: Parses the legacy Vanilla model and surgically extracts only the 3D Mesh Container (`0x02610`).
3. **Texture Register Translation**: Injects Vanilla raw texture/palette data and dynamically patches the low-level GS `TEX0` register bitfields (dynamically re-calculating log2 `TW` and `TH` power-of-two texture dimensions) to conform to Frontiers standards.
4. **Tree Reconstruction**: Merges the Vanilla Mesh and translated textures into the Frontiers skeletal base, updating all recursive node size headers and generating a perfect hybrid model.
5. **UDF / LBA Repacking**: Recompiles the massive `CHAR.ESF` database and patches the raw ISO 9660 Directory Records, Primary Volume Descriptors, and UDF Allocation Descriptors to ensure the emulator reads the new logical block bounds flawlessly.

## 🛠️ Features & Tooling

- **`vanilla_to_frontiers_transplant.py`**: The core reverse-engineering script that handles the byte-level graft logic.
- **Dynamic ESF/UDF Repacker**: Automatically writes modified assets into the ISO and repairs disc sector LBA pointers.
- **Live PS2 RAM Tracer (`live_ram_tracer.py`)**: Dynamically hooks into a running `pcsx2-qt.exe` process via Win32 APIs, locates the 32MB contiguous Emotion Engine (EE) Main RAM block, and actively scans for loaded `0x72700` node trees to diagnose active memory corruption.

## 🚀 Getting Started & Environment Setup

We have made it incredibly easy for new contributors to jump in. You do not need to hunt down the original game files.

1. Clone or download this repository.
2. Double-click **`setup_environment.bat`**.
3. The script will automatically download the required baseline ISOs (~10GB total) directly into the correct folders:
   - `iso/unmodified/EQOA_Original.iso` (Vanilla)
   - `iso/unmodified/EQOA_Frontiers.iso` (Expansion)
   - `iso/legacy/EQOA_FRONTIERS_PREVIOUS_CONTRACTOR.iso` (A custom build from a previous contractor that fails to render, provided for historical context/reference).

Once the setup script finishes, you are ready to start patching and diagnosing!

## 🎮 Usage 

The entire suite is abstracted into a single automated interface.

1. Ensure you have run `setup_environment.bat`.
2. Double-click **`EQOA_MASTER_TOOL.bat`**.

### Options:
- **[1] Patch Game ISO**: Executes the hybrid graft pipeline. It parses the databases, surgically upgrades 11 target character assets, repacks the `CHAR.ESF`, and generates a bootable `iso/patched/EQOA_Frontiers_Patched.iso`.
- **[2] Run Live RAM Diagnostics**: If the emulator is currently running the patched game, this will hook into the PCSX2 process, scan the EE memory for 30 seconds, output live tree structures of loaded models, and compile exactly two logs (`latest_diagnostic_log.txt` and `history_diagnostic_log.txt`) in `/diagnostics/logs/`.

## 📁 Repository Architecture

```text
EQOA-Character-Restoration/
├── EQOA_MASTER_TOOL.bat            # Primary execution interface
├── README.md                       # Technical documentation
├── core/                           # Primary application logic
│   ├── vanilla_to_frontiers_transplant.py # Hybrid Graft Logic
│   ├── esf_parser.py               # Reverse-engineered binary parser
│   └── repack_iso.py               # UDF/ISO9660 LBA Patcher
├── diagnostics/                    # Live Memory Analysis Tools
│   ├── diagnostic_suite.py         # Diagnostic Orchestrator
│   ├── live_ram_tracer.py          # EEmem 32MB contiguous scanner
│   └── logs/                       # Automated log output directory
├── docs/                           # Deep-dive guides and reports
├── iso/                            
│   └── patched/                    # Final playable PS2 image
├── legacy/                         # Deprecated bash/batch files
└── workspace/                      # Raw binary dumps and payloads
```
