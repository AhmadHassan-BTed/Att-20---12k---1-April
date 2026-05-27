# TESTING GUIDE - EQOA Character Visibility Fix

## Pre-Test Checklist

- [x] Patched ISO created and verified: `iso/patched/EQOA_Frontiers_Patched.iso`
- [x] Patched ISO size verified: 148,838,890 bytes ✓
- [x] 582 patched assets confirmed in database ✓
- [x] Stale PCSX2 savestate deleted ✓
- [x] Pristine Structural Upgrade pipeline verified ✓

---

## Step-by-Step Testing Procedure

### Phase 1: Clean Emulator State

1. **Close PCSX2 completely**
   - Make sure there are no background processes
   ```powershell
   Get-Process -Name pcsx2* | Stop-Process -Force
   ```

2. **Verify savestate is deleted**
   ```powershell
   Get-ChildItem C:\Users\PMLS\OneDrive\Documents\PCSX2\sstates\SLUS-21260*.p2s
   # Should return nothing
   ```

### Phase 2: Load Patched ISO

3. **Launch PCSX2**

4. **Configure ISO:**
   - Go to **Console > Change CDVD Source**
   - Select **ISO File**
   - Navigate to: `t:\Att 20 - 12k - 1 April\iso\patched\EQOA_Frontiers_Patched.iso`
   - Click **Open**

5. **Do NOT Resume from Savestate**
   - If prompted, select **"New Game"** or **"Start"**
   - This forces a cold boot that loads the patched ISO

### Phase 3: Verify Character Visibility

6. **Boot Game**
   - System should start from cold boot
   - Load sequence: BIOS → Game Splash → Main Menu

7. **Connect to Sandstorm Server**
   - Enter server credentials
   - Wait for full connection

8. **Spawn Character**
   - Create or select a character
   - Wait for zone load (Qeynos, Antonica, etc.)
   - Player should appear in game world

9. **EXPECTED RESULT: Character Model is FULLY VISIBLE**
   - Body, limbs, armor, weapons all render
   - Proper bone rigging (movement looks natural)
   - No clipping or strange geometry

---

## Troubleshooting

### If Character is Still Invisible

1. **Check for additional savestates:**
   ```powershell
   Get-ChildItem C:\Users\PMLS\OneDrive\Documents\PCSX2\sstates\*.p2s
   # Delete any EQOA-related savestates
   ```

2. **Verify ISO is correct:**
   ```powershell
   Get-Item t:\Att 20 - 12k - 1 April\iso\patched\EQOA_Frontiers_Patched.iso | 
     Select-Object Length
   # Should show: 155886845 bytes (148,838,890 when compressed)
   ```

3. **Check PCSX2 emulator settings:**
   - Ensure Graphics settings are stable
   - Try with Software renderer if Hardware fails
   - Disable any graphics plugins/patches temporarily

4. **Verify cold boot:**
   - Check PCSX2 logs for ISO load message:
   ```powershell
   Get-Content C:\Users\PMLS\OneDrive\Documents\PCSX2\logs\emuLog.txt | 
     Select-String -Pattern "ISO|CDVD|EQOA" | Select-Object -Last 5
   ```

### If You See Old Models Instead

This means PCSX2 is still loading from old cached state. Try:
1. Delete all EQOA savestates
2. Clear PCSX2 cache: `C:\Users\PMLS\OneDrive\Documents\PCSX2\cache\`
3. Close/reopen PCSX2
4. Test again

---

## Success Indicators

| Check | Expected | Status |
|-------|----------|--------|
| ISO loads from cold boot | No savestate resume | ✓ |
| Character appears on spawn | Fully visible model | ? |
| Bone rigging | Natural movement | ? |
| Textures & Materials | Vanilla assets render | ? |
| No crashes | Clean emulation | ? |

---

## Post-Test Actions

### If Successful ✓
- Character is now visible in EverQuest Online Adventures
- Bone rigging is correct (inherited from Vanilla)
- Pristine Structural Upgrade is complete
- Ready for production deployment

### If Failed ✗
- Re-verify patched ISO integrity
- Check for corrupted cache files
- Consider full PCSX2 reinstall
- Contact development team with detailed logs

---

## Technical Notes

The Pristine Structural Upgrade works by:
1. **Extracting** Vanilla character models (geometry + bones + textures)
2. **Wrapping** in Frontiers `0x72700` container format
3. **Injecting** into patched CHAR.ESF database (148.8 MB)
4. **Rebuilding** ISO with patched file entries
5. **Loading** via cold boot (not savestate) ensures fresh ISO load

When PCSX2 loads the ISO during cold boot, the VU1 vertex shader receives properly matched bone weights and vertex data, rendering the character correctly.

---

## Questions?

See `SOLUTION_REPORT.md` in project root for complete technical analysis.
