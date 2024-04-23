@echo off
SET USE_ADMIN_ENV=false
net session >nul 2>&1
if %errorLevel% == 0 (
   SET USE_ADMIN_ENV=true
)
echo wait...

SET "SDK_LIB_PATH=%~1"
if "%SDK_LIB_PATH%"=="" (
   SET "SDK_LIB_PATH=%~dp0SDKlib"
)

SET USE_ADMIN_ENV=false
SET "CMAKE_INSTALL_PATH=%~dp0"

echo start: make and pack GenTLwrapper
mkdir "%~dp0SDKcode\GenTLwrapper\build"
cd SDKcode\GenTLwrapper\build
cmake .. -A x64 -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PATH%"
cmake --build . --config Release -j 
cmake --install .
cd ..\..\..\
@RD /S /Q "%~dp0SDKcode\GenTLwrapper\build"
echo end: make and pack GenTLwrapper

if exist "%CMAKE_INSTALL_PATH%Vidu_SDK_Api\SDKlib\GenTLwrapper.dll" (
   echo SDK_LIB_PATH set
   SET "SDK_LIB_PATH=%CMAKE_INSTALL_PATH%Vidu_SDK_Api\SDKlib"
) else (
   echo "%CMAKE_INSTALL_PATH%SDKlib\GenTLwrapper.dll" not exist
   echo GenTLwrapper build failed
)
echo SDK_LIB_PATH "%SDK_LIB_PATH%"
setx DYVCAM_GENTL64_PATH "%SDK_LIB_PATH%"
Powershell.exe -executionpolicy Bypass -Command "& '%~dp0camdriver\set_env_path.ps1' '%SDK_LIB_PATH%' 'ViduSdk-;ViduSdk_Internal-;Vidu_SDK_Api\SDKlib;DYV_SDK_Api\SDKlib;ViduSdk\SDKlib;ViduSdk_Internal\SDKlib;..\build\lib;OkuloSdk-windows\SDKlib;OkuloSdk\SDKlib;OkuloSdk_Internal\SDKlib' 'PATH' '%USE_ADMIN_ENV%'"
Powershell.exe -executionpolicy Bypass -Command "& '%~dp0camdriver\set_env_path.ps1' '%SDK_LIB_PATH%\SDKpythonLib' 'ViduSdk-;ViduSdk_Internal-;Vidu_SDK_Api\SDKlib\SDKpythonLib;ViduSdk\SDKlib\SDKpythonLib;ViduSdk_Internal\SDKlib\SDKpythonLib;..\build\lib' 'PYTHONPATH' '%USE_ADMIN_ENV%'"
SET "script_path=%~dp0camdriver\okulo_c1_metadata_win.ps1"
Powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& {Start-Process Powershell.exe -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File ""%script_path%""' -Verb RunAs}"
echo finish, need to reopen the console window!