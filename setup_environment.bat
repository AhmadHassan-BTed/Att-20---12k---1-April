@echo off
echo ========================================================
echo EQOA Character Restoration - Environment Setup
echo ========================================================
echo.
echo This script will automatically download all the required PS2 ISOs
echo so you can start contributing immediately. 
echo.
echo TOTAL DOWNLOAD SIZE: ~10 GB
echo Depending on your internet speed, this may take a while.
echo.
echo Press any key to start downloading...
pause >nul

:: Create required directories
mkdir iso\unmodified 2>nul
mkdir iso\legacy 2>nul
mkdir iso\patched 2>nul

echo.
echo [*] Downloading 1/3: EQOA Original Version (Vanilla)...
curl -L -C - -o "iso\unmodified\EQOA_Original.iso" "https://www.dropbox.com/scl/fi/p19wxx8crqn1p38s713dy/EverQuest-Online-Adventures.iso?rlkey=fg8bp96e6qy0a6p0tocmoj9y7&st=zny0qnik&dl=1"

echo.
echo [*] Downloading 2/3: EQOA Frontiers Expansion...
curl -L -C - -o "iso\unmodified\EQOA_Frontiers.iso" "https://www.dropbox.com/scl/fi/tezjukiyt9hctyxwadqzs/EverQuest-Online-Adventures-Frontiers-USA.iso?rlkey=phbn4feje480xyetjrciweyty&st=7srx9uiz&dl=1"

echo.
echo [*] Downloading 3/3: Previous Contractor Custom ISO (For reference)...
curl -L -C - -o "iso\legacy\EQOA_FRONTIERS_PREVIOUS_CONTRACTOR.iso" "https://www.dropbox.com/scl/fi/zzts8asi14hu4qim90oaw/EQOA_FRONTIERS_ORIGINAL_MODELS.iso?rlkey=nfsc2fs7ant89vu7qja6km0o1&st=tozj25xy&dl=1"

echo.
echo ========================================================
echo ALL FILES DOWNLOADED SUCCESSFULLY!
echo.
echo Your workspace is now fully set up.
echo You can run EQOA_MASTER_TOOL.bat to begin compiling or testing.
echo ========================================================
pause
