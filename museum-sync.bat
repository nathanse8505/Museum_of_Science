@echo off
REM Get the folder where this .bat lives
set "SCRIPT_DIR=%~dp0"

REM Ensure execution policy is at least RemoteSigned for CurrentUser
powershell.exe -NoProfile -Command ^
 "if ((Get-ExecutionPolicy -Scope CurrentUser) -eq 'Restricted') { Set-ExecutionPolicy -Scope CurrentUser RemoteSigned -Force }"

REM Run PowerShell script from that folder
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%museum-sync.ps1"
