# EQOA PS2 Frontiers Custom Patch Guide

This guide describes how to patch and restore original character models and custom assets into the PlayStation 2 *EverQuest Online Adventures: Frontiers* game ISO using this fully automated reverse-engineering toolkit.

## 🚀 Pre-requisites & Setup

1. **Python Environment**: Ensure you have Python installed (version 3.10 or higher is recommended).
2. **Setup Workspace**: Double-click **`setup_environment.bat`** to automatically create the folder structures and download the required baseline clean game ISO files:
   - `iso/unpatched/EQOA_Original.iso` (Original game disc)
   - `iso/unpatched/EQOA_Frontiers.iso` (Frontiers expansion disc)

*(Alternatively, if you already have clean unpatched copies of these ISOs, you can place them manually into the `iso/unpatched/` folder under these exact names).*

## 📁 Custom Assets Placement (Simplified Assets Directory)

The pipeline organizes assets under a single parent **`assets/`** directory to keep baseline resources and custom overlays clean and distinct:

* **`assets/Vanilla/`**: Contains the baseline Vanilla game assets (character models, custom select files, etc.).
* **`assets/Frontiers/`**: Contains baseline Frontiers assets or your custom overlay patches.

### Directory Structure & File Naming:
Within each folder under `assets/`, files must be structured as follows:
* **`data/`**:
  - `CHAR.ESF` — Character model database.
  - `CHARCUST.ESF` — Character customizer database.
  
* **`data2/`**:
  - `CHARCUST.CSF` — Compressed character customizer database.
  - `CHARFACE.CSF` — Compressed character face database.
  - `CHARFACE.ESF` — Character face database.
  - `CHARSEL1.CSF`, `CHARSEL2.CSF`, `CHARSEL3.CSF`, `CHARSEL4.CSF` — Compressed character select files.

*(Note: Baseline assets are preloaded in `assets/Vanilla/`. Step 2 extracts clean baseline files into `assets/Frontiers/` so you can customize them).*

## ⚙️ Running the 4-Step Automated Patch Pipeline

To patch the game ISO and inject custom assets, run the following four batch scripts in order:

### 1️⃣ Step 1: Create Initial Frontiers Patched ISO (`steps/step1_create_patched_iso.bat`)
- **What it does**: Re-compiles the 11 native character model databases (Vanilla geometry grafted onto Frontiers skeleton) and repacks them into a baseline frontiers ISO (`iso/patched/EQOA_Frontiers_Patched.iso`).

### 2️⃣ Step 2: Extract Baseline Frontiers Assets (`steps/step2_extract_assets.bat`)
- **What it does**: Automatically extracts baseline Frontiers CSF/ESF database files directly from your clean unpatched Frontiers ISO and saves them into the `assets/Frontiers/` directory. This is useful for customizing or referencing raw Frontiers assets.

### 3️⃣ Step 3: Merge Assets (`steps/step3_merge_assets.bat`)
- **What it does**: Merges baseline Vanilla assets from `assets/Vanilla/` and custom Frontiers overlays from `assets/Frontiers/` into a temporary `assets/merged-assets/` folder. (Custom Frontiers files take priority and overwrite matching Vanilla files).

### 4️⃣ Step 4: Inject Assets (`steps/step4_inject_assets.bat`)
- **What it does**: Forcefully terminates any running `pcsx2-qt.exe` process (to prevent file lock conflicts), copies combined assets from `assets/merged-assets/` into the workspace folders, and surgically patches them in-place directly into the patched ISO. It concludes by running a high-integrity verification suite.

## 🎮 Playing the Game

Once the tool reports **`HIGH-FIDELITY STRUCTURAL TRANSPLANT PIPELINE EXECUTED SUCCESSFULLY`**:
1. Open the PCSX2 emulator.
2. Select and load: **`iso/patched/EQOA_Frontiers_Patched.iso`**.
3. The custom assets and character models will render in-game with zero visual artifacts or engine rendering crashes.
