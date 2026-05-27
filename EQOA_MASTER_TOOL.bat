@echo off
:: Check for Administrator privileges automatically
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [*] Requesting Administrator privileges...
    powershell -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
echo ========================================================
echo EQOA MASTER TOOL
echo ========================================================
echo.
echo Please select an action:
echo.
echo [1] Patch Game ISO (Run this first, with emulator closed)
echo [2] Run Full Diagnostics, Logging ^& Testing (Run this while in-game)
echo [3] Exit
echo.
set /p choice="Enter your choice (1, 2, or 3): "

if "%choice%"=="1" (
    echo.
    echo [*] Running Pristine Structural Transplant Pipeline...
    python -m core.vanilla_to_frontiers_transplant
    echo.
    echo ========================================================
    echo ALL DONE! Your new game file is: iso/patched/EQOA_Frontiers_Patched.iso
    echo ========================================================
    pause
) else if "%choice%"=="2" (
    echo.
    python diagnostics\diagnostic_suite.py
    pause
) else (
    exit
)
