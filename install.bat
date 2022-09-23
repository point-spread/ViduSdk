@echo off
SET USE_ADMIN_ENV=false
net session >nul 2>&1
if %errorLevel% == 0 (
   SET USE_ADMIN_ENV=true
)

setx DYVCAM_GENTL64_PATH "%~dp0SDKlib"
Powershell.exe -executionpolicy Bypass -Command "& '%~dp0camdriver/setEnvPath.ps1' %DYVCAM_GENTL64_PATH% '%USE_ADMIN_ENV%'"