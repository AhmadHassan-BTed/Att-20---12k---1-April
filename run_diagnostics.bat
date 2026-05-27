@echo off
REM Master Diagnostic Runner Batch Script
REM Runs all EQOA character visibility diagnostics in one command

setlocal enabledelayedexpansion
set PYTHONPATH=.

echo.
echo ============================================================
echo   EQOA DIAGNOSTICS - Character Visibility Troubleshooting
echo ============================================================
echo.

python run_all_diagnostics.py

if errorlevel 1 (
    echo.
    echo [ERROR] Diagnostics encountered errors
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo [SUCCESS] All diagnostics completed
    echo.
)

endlocal
pause
