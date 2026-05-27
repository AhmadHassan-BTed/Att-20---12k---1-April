@echo off
echo ========================================================
echo EQOA Live Memory Diagnostics Suite - Administrator Mode
echo ========================================================
echo.

:: Check for admin rights
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Requesting Administrator privileges...
    powershell -Command "Start-Process cmd -ArgumentList '/c cd /d \"%~dp0\" && python diagnostics\diagnostic_suite.py & pause' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
python diagnostics\diagnostic_suite.py
pause
