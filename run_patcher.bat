@REM @echo off
@REM echo ========================================================
@REM echo EQOA Frontiers - Asset Restoration Patcher
@REM echo ========================================================
@REM echo.

@REM :: Fix double extensions caused by Windows hiding known file types
@REM if exist "EQOA_Original.iso.iso" rename "EQOA_Original.iso.iso" "EQOA_Original.iso"
@REM if exist "EQOA_Frontiers.iso.iso" rename "EQOA_Frontiers.iso.iso" "EQOA_Frontiers.iso"

@REM echo Please ensure you have placed your original ISO files here:
@REM echo - EQOA_Original.iso
@REM echo - EQOA_Frontiers.iso
@REM echo.

@REM if not exist "EQOA_Original.iso" (
@REM     echo [ERROR] Could not find EQOA_Original.iso in this folder!
@REM     pause
@REM     exit /b
@REM )

@REM if not exist "EQOA_Frontiers.iso" (
@REM     echo [ERROR] Could not find EQOA_Frontiers.iso in this folder!
@REM     pause
@REM     exit /b
@REM )

@REM echo [*] Installing required Python libraries...
@REM pip install construct pycdlib

@REM echo.
@REM echo [*] Step 1: Extracting game data...
@REM python extract_assets.py

@REM echo.
@REM echo [*] Step 2: Isolating character models...
@REM python payload_extractor.py

@REM echo.
@REM echo [*] Step 3: Transplanting Vanilla textures into native Frontiers skeletons...
@REM python frankenstein_texture_swapper.py

@REM echo.
@REM echo [*] Step 4: Rebuilding model database...
@REM python esf_rebuilder.py

@REM echo.
@REM echo [*] Step 5: Generating Patched ISO...
@REM python repack_iso.py

@REM echo.
@REM echo ========================================================
@REM echo ALL DONE! 
@REM echo Your new game file is: EQOA_Frontiers_Patched.iso
@REM echo You can now load this ISO in PCSX2 and play the game!
@REM echo ========================================================
@REM pause

@echo off
echo ========================================================
echo EQOA Frontiers - Asset Restoration Patcher (Fixed Pipeline)
echo ========================================================
echo.

:: We are Muzzling extraction to prevent overwriting your manual grafts
echo [*] WARNING: Extraction steps are Muzzled to preserve grafted assets.
echo [*] Ensure grafted .bin files are in /workspace/payloads/
echo.

:: [Step 1 & 2 REMOVED to prevent overwriting manual Frankenstein grafts]
echo [*] Step 1 & 2: Skipping Extraction (Preserving manual binary grafts)...
echo.

echo [*] Step 3: Transplanting Vanilla textures into native Frontiers skeletons...
python frankenstein_texture_swapper.py

echo.
echo [*] Step 4: Rebuilding model database...
python esf_rebuilder.py

echo.
echo [*] Step 5: Generating Patched ISO...
python repack_iso.py

echo.
echo ========================================================
echo ALL DONE! 
echo Your new game file is: EQOA_Frontiers_Patched.iso
echo ========================================================
pause