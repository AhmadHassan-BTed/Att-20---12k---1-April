@echo off
echo ========================================================
echo EQOA Live RAM Tracer - Administrator Mode
echo ========================================================
echo.

:: Check for admin rights
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Requesting Administrator privileges...
    powershell -Command "Start-Process cmd -ArgumentList '/c cd /d \"%~dp0\" && python diagnostics\live_ram_tracer.py --scan-models --continuous & pause' -Verb RunAs"
    exit /b
)

cd /d "%~dp0"
python diagnostics\live_ram_tracer.py --scan-models --continuous
pause
