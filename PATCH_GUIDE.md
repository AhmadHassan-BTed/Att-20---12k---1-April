# EQOA PS2 Frontiers Custom Patch Guide

This guide describes how to patch and restore original character models and custom assets into the PlayStation 2 *EverQuest Online Adventures: Frontiers* game ISO using this fully automated reverse-engineering toolkit.

## 🚀 Pre-requisites & Setup

1. **Python Environment**: Ensure you have Python installed (version 3.10 or higher is recommended).
2. **Setup Workspace**: Double-click **`setup_environment.bat`** to automatically create the folder structures and download the required baseline clean game ISO files:
   - `iso/unpatched/EQOA_Original.iso` (Original game disc)
   - `iso/unpatched/EQOA_Frontiers.iso` (Frontiers expansion disc)

*(Alternatively, if you already have clean unpatched copies of these ISOs, you can place them manually into the `iso/unpatched/` folder under these exact names).*

## 📁 Custom Assets Placement

The pipeline is pre-configured to automatically inject any assets placed in the **`assets/`** directory. If you have custom versions of these assets, ensure they are placed as follows:

* **`assets/data/`**:
  - `CHAR.ESF` — Custom character model database.
  - `CHARCUST.ESF` — Custom character customizer database.
  
* **`assets/data2/`**:
  - `CHARCUST.CSF` — Compressed character customizer database.
  - `CHARFACE.CSF` — Compressed character face database.
  - `CHARFACE.ESF` — Character face database.
  - `CHARSEL1.CSF`, `CHARSEL2.CSF`, `CHARSEL3.CSF`, `CHARSEL4.CSF` — Compressed character select files.

*(Note: Pre-packaged fully verified custom assets are already included in the `assets/` directory in this repository by default).*

## ⚙️ Running the Automated Patch Pipeline

1. Ensure the PCSX2 emulator is **closed** to prevent file lock conflicts on the ISO files.
2. Double-click **`EQOA_MASTER_TOOL.bat`**.
3. Choose Option **`[1] Patch Game ISO`** and hit enter.

### What the Automated Tool Does:
- **Phase 1**: Commences low-level clean graft surgery to re-compile the 11 character model databases.
- **Phase 2**: Overwrites the corresponding workspace files with the placed files inside the `assets/` folder.
- **Phase 3**: Copies the unpatched base ISO and appends the recompiled databases and custom CSF/ESF asset payloads to the end of the partition.
- **Phase 4**: Surgically byte-patches all **ISO 9660 Directory Records** and **UDF File Entries (FEs)** in-place with the exact new sector LBAs and sizes.
- **Phase 5**: Aligns the Primary Volume Descriptor (PVD) sector count, appends the UDF AVDP sector at the final sector boundary, and runs a comprehensive validation suite to ensure 100% data integrity and readable sectors.

## 🎮 Playing the Game

Once the tool reports **`HIGH-FIDELITY STRUCTURAL TRANSPLANT PIPELINE EXECUTED SUCCESSFULLY`**:
1. Open the PCSX2 emulator.
2. Select and load: **`iso/patched/EQOA_Frontiers_Patched.iso`**.
3. The custom assets and character models will render in-game with zero visual artifacts or engine rendering crashes.
