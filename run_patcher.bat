@echo off
echo ========================================================
echo EQOA Frontiers - Asset Restoration Patcher
echo ========================================================
echo.
echo Please ensure you have placed your original ISO files here:
echo - EQOA_Original.iso
echo - EQOA_Frontiers.iso
echo.
pause

echo.
echo [*] Installing required Python libraries...
pip install construct pycdlib

echo.
echo [*] Step 1: Extracting game data...
python extract_assets.py

echo.
echo [*] Step 2: Isolating character models...
python payload_extractor.py

echo.
echo [*] Step 3: Rebuilding model database...
python esf_rebuilder.py

echo.
echo [*] Step 4: Generating Patched ISO...
python repack_iso.py

echo.
echo ========================================================
echo ALL DONE! 
echo Your new game file is: EQOA_Frontiers_Patched.iso
echo You can now load this ISO in PCSX2 and play the game!
echo ========================================================
pause
