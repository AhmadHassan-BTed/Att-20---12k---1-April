@echo off
echo ========================================================
echo EQOA Frontiers - Asset Restoration Patcher
echo ========================================================
echo.

:: Fix double extensions caused by Windows hiding known file types
if exist "EQOA_Original.iso.iso" rename "EQOA_Original.iso.iso" "EQOA_Original.iso"
if exist "EQOA_Frontiers.iso.iso" rename "EQOA_Frontiers.iso.iso" "EQOA_Frontiers.iso"

echo Please ensure you have placed your original ISO files here:
echo - EQOA_Original.iso
echo - EQOA_Frontiers.iso
echo.

if not exist "EQOA_Original.iso" (
    echo [ERROR] Could not find EQOA_Original.iso in this folder!
    pause
    exit /b
)

if not exist "EQOA_Frontiers.iso" (
    echo [ERROR] Could not find EQOA_Frontiers.iso in this folder!
    pause
    exit /b
)

echo [*] Installing required Python libraries...
pip install construct pycdlib

echo.
echo [*] Step 1: Extracting game data...
python extract_assets.py

echo.
echo [*] Step 2: Isolating character models...
python payload_extractor.py

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
echo You can now load this ISO in PCSX2 and play the game!
echo ========================================================
pause
