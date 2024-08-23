@echo off
SET USE_ADMIN_ENV=false
net session >nul 2>&1
if %errorLevel% == 0 (
   SET USE_ADMIN_ENV=true
)
echo wait...
echo "Install adb tool to %USERPROFILE%"
tar -xzvf camdriver\win_adb.tar.gz --strip-components 1 -C %USERPROFILE%\ > nul 2>&1

SET "SDK_LIB_PATH=%~1"
if "%SDK_LIB_PATH%"=="" (
   SET "SDK_LIB_PATH=%~dp0Vidu_SDK_Api\SDKlib"
)
echo SDK_LIB_PATH "%SDK_LIB_PATH%"

SET USE_ADMIN_ENV=false
Powershell.exe -executionpolicy Bypass -Command "& '%~dp0camdriver\set_env_path.ps1' '%USERPROFILE%' 'Vidu_SDK_Api' 'PATH' '%USE_ADMIN_ENV%'"
SET "script_path=%~dp0camdriver\okulo_c1_metadata_win.ps1"
Powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& {Start-Process Powershell.exe -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File ""%script_path%""' -Verb RunAs}"
setx DYVCAM_GENTL64_PATH "%SDK_LIB_PATH%"
echo finish, need to reopen the console window!
