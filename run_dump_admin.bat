@echo off
echo ========================================================
echo EQOA RAM Transition Dumper - Administrator Mode
echo ========================================================
echo.

:: Check for admin rights
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Requesting Administrator privileges...
    powershell -Command "Start-Process cmd -ArgumentList '/c cd /d \"%~dp0\" && python workspace\scratch\dump_active_b070.py & pause' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
python workspace\scratch\dump_active_b070.py
pause
