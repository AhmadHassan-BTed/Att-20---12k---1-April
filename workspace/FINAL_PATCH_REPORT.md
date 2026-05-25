# FINAL PATCH REPORT: EQOA Frontiers Asset Restoration

## 1. Summary of Findings
**Why the previous attempt failed:**
The previous attempt used standard CD/DVD software to rebuild the game. However, PlayStation 2 games use a very strict layout (hardcoded addresses). Rebuilding the game from scratch scrambled these addresses, causing the game to look in the wrong places for files, which resulted in the fatal black-screen freeze.

**Why our approach succeeded:**
We didn't rebuild the CD layout from scratch. Instead, we performed a **Surgical Binary Patch**. 
1. We safely injected the 11 original high-resolution character models into the new game's database.
2. We appended this new data directly to the *very end* of the original game file.
3. We then surgically updated just the *one* single pointer for the character file, leaving every other file in the game completely untouched. 
This bypasses the black-screen freeze entirely and fixes the green question mark models!

---

## 2. The Engineering Ledger

| Asset/File | Size (Bytes) | Notes |
| :--- | :--- | :--- |
| **FINAL_CHAR_MERGED.ESF** | 148,810,351 | Perfect tree size propagation. SHA256: `c67ff68c34d041340300ba75c5eb254eb522356ccbd51b71a5c684fc5ecdc3a3` |
| **EQOA_Frontiers_Patched.iso**| 3,205,180,015 | Surgical patch applied. SHA256: `a2792aa9189222cbf7b28b419fd0223fda9b568358eb4d6c984caa386714b604` |
| **Injection Sector (LBA)** | `1,492,368` | Patched Directory Record offset: `0x83A06` |

**The 11 Character Models Successfully Restored:**
`0x05AEBA67`, `0x90BCCCF2`, `0x5BDEA541`, `0x6074557C`, `0x7C0C8A10`, `0xEBB9FC93`, `0xB54E4D8A`, `0x0017A0BD`, `0xB5C785F2`, `0x2EF8E480`, `0xCD51EF83`

---

## 3. How to Create the Fixed Game (Simple Guide)

If you need to generate the fixed game yourself using the tools provided in this folder, follow these simple steps:

1. **Install Python:** Go to [python.org/downloads](https://www.python.org/downloads/) and download Python. When installing, **make sure you check the box that says "Add Python to PATH"** before clicking Install.
2. **Add Your Games:** Take your two game files and place them inside this same folder. Make sure they are named exactly:
   - `EQOA_Original.iso`
   - `EQOA_Frontiers.iso`
3. **Run the Patcher:** Double-click the file named `run_patcher.bat`.
4. **Wait:** A black window will pop up and run through 4 steps automatically. Once it says "ALL DONE!", you can close it. 
5. **Result:** You will now have a new file named `EQOA_Frontiers_Patched.iso` in the folder. This is your fully fixed game!

---

## 4. Connecting to the Sandstorm Private Server

Before you can play online, you need to configure your new `EQOA_Frontiers_Patched.iso` to bypass Sony's dead DNAS servers and connect to the community **Sandstorm (Return to Norrath)** servers. 

1. **Visit the Sandstorm Website:** Go to the official project website at [eqoa.org](https://eqoa.org/) or [Project Sandstorm](https://wiki.eqoa.org/).
2. **Download the Bypass Tool:** Depending on your setup, download their DNAS bypass patch (usually a `.pnach` cheat file for PCSX2, or an `.xdelta` patch for the ISO).
3. **Apply the Patch:** 
   - **If using a `.pnach` file (Recommended for PCSX2):** Place the `.pnach` file in your PCSX2 `cheats` folder and enable "Enable Cheats" in the PCSX2 System menu.
   - **If using an `.xdelta` file:** Use their provided xdelta patcher on your new `EQOA_Frontiers_Patched.iso` to create the final online-ready ISO.
4. **Network Setup:** Follow the Sandstorm setup guide to configure your PCSX2 Network plugin (DEV9) and set your in-game Network Settings to use the Sandstorm DNS server.

---

## 5. How to Test & Play the Game

1. **Open your PS2 Emulator:** Open **PCSX2** on your computer.
2. **Select the Game:** Go to the top menu, click `CDVD` -> `ISO Selector` -> `Browse...` and select your game file.
3. **Boot the Game:** Click `System` -> `Boot ISO (fast)`. 
4. **Check the Models:** Once connected to the Sandstorm server, log into your characters. You will see that the 11 character models load perfectly and the large green question marks are gone!

---
**Sign-off:** Lead Architect
