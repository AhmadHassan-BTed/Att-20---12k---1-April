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
echo [1] Patch Game ISO (Static Mesh T-Pose Mode) (Run with emulator closed)
echo [2] Run Full Diagnostics, Logging ^& Testing (Run this while in-game)
echo [3] Exit
echo.
set /p choice="Enter your choice (1, 2, or 3): "

if "%choice%"=="1" (
    echo.
    echo [*] Running Pristine Structural Transplant Pipeline...
    python -m core.vanilla_to_frontiers_transplant
    if %errorlevel% neq 0 exit /b %errorlevel%
    echo.
    python core\verify_final_iso.py
    if %errorlevel% neq 0 exit /b %errorlevel%
) else if "%choice%"=="2" (
    echo.
    python diagnostics\diagnostic_suite.py
    pause
) else (
    exit
)
