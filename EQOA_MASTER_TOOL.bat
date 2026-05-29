@echo off
cd /d "%~dp0"

:menu
cls
echo ========================================================
echo               EQOA CUSTOM PATCH MASTER TOOL
echo ========================================================
echo.
echo Please select a step to run:
echo.
echo [1] Step 1: Create Initial Frontiers Patched ISO
echo [2] Step 2: Extract Baseline Frontiers Assets to assets/Frontiers
echo [3] Step 3: Merge Vanilla and Frontiers Assets
echo [4] Step 4: Inject Assets into Patched ISO and Verify
echo [5] Run All Steps Sequentially (Full Auto-Patch Pipeline)
echo [6] Run Full Diagnostics (Diagnostics Suite)
echo [7] Exit
echo.
set /p choice="Enter your choice (1-7): "

if "%choice%"=="1" goto step1
if "%choice%"=="2" goto step2
if "%choice%"=="3" goto step3
if "%choice%"=="4" goto step4
if "%choice%"=="5" goto runall
if "%choice%"=="6" goto diagnostics
if "%choice%"=="7" goto end
goto menu

:step1
echo.
echo [*] Running Step 1: Create Initial Frontiers Patched ISO...
python -m core.vanilla_to_frontiers_transplant
if %errorlevel% neq 0 (
    echo.
    echo [-] Step 1 Failed!
    pause
    goto menu
)
echo.
echo [SUCCESS] Step 1 Completed Successfully!
pause
goto menu

:step2
echo.
echo [*] Running Step 2: Extract Baseline Frontiers Assets...
python -m core.extract_frontiers_assets
if %errorlevel% neq 0 (
    echo.
    echo [-] Step 2 Failed!
    pause
    goto menu
)
echo.
echo [SUCCESS] Step 2 Completed Successfully!
pause
goto menu

:step3
echo.
echo [*] Running Step 3: Merge Vanilla and Frontiers Assets...
python -m core.merge_assets
if %errorlevel% neq 0 (
    echo.
    echo [-] Step 3 Failed!
    pause
    goto menu
)
echo.
echo [SUCCESS] Step 3 Completed Successfully!
pause
goto menu

:step4
echo.
echo [*] Running Step 4: Inject Assets into Patched ISO...
tasklist | findstr /I "pcsx2-qt.exe" >nul
if %errorlevel% equ 0 (
    echo [*] Detected running PCSX2 emulator. Closing it to prevent file locks...
    taskkill /F /IM pcsx2-qt.exe >nul
)
python -m core.patch_placed_assets
if %errorlevel% neq 0 (
    echo.
    echo [-] Step 4 Failed!
    pause
    goto menu
)
echo.
echo [*] Running high-integrity verification suite...
python -m core.verify_final_patch
if %errorlevel% neq 0 (
    echo.
    echo [-] Verification Failed!
    pause
    goto menu
)
echo.
echo [SUCCESS] Step 4 Completed Successfully!
pause
goto menu

:runall
echo.
echo ========================================================
echo RUNNING FULL AUTO-PATCH PIPELINE (ALL STEPS)
echo ========================================================
echo.
echo [*] Starting Step 1/4...
python -m core.vanilla_to_frontiers_transplant
if %errorlevel% neq 0 goto runall_failed

echo.
echo [*] Starting Step 2/4...
python -m core.extract_frontiers_assets
if %errorlevel% neq 0 goto runall_failed

echo.
echo [*] Starting Step 3/4...
python -m core.merge_assets
if %errorlevel% neq 0 goto runall_failed

echo.
echo [*] Starting Step 4/4...
tasklist | findstr /I "pcsx2-qt.exe" >nul
if %errorlevel% equ 0 (
    echo [*] Detected running PCSX2 emulator. Closing it to prevent file locks...
    taskkill /F /IM pcsx2-qt.exe >nul
)
python -m core.patch_placed_assets
if %errorlevel% neq 0 goto runall_failed
python -m core.verify_final_patch
if %errorlevel% neq 0 goto runall_failed

echo.
echo ========================================================
echo [SUCCESS] ALL PIPELINE STEPS EXECUTED SUCCESSFULLY!
echo ========================================================
pause
goto menu

:runall_failed
echo.
echo [-] Pipeline failed at one of the stages! Please inspect logs above.
pause
goto menu

:diagnostics
echo.
python diagnostics\diagnostic_suite.py
pause
goto menu

:end
exit
