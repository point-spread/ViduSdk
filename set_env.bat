@echo off
SET "SDK_LIB_PATH=%cd%\SDKlib"
setx DYVCAM_GENTL64_PATH "%SDK_LIB_PATH%"
Powershell.exe -executionpolicy Bypass -Command "& '%cd%\camdriver\set_env_path.ps1' '%SDK_LIB_PATH%' 'ViduSdk-;ViduSdk_Internal-;Vidu_SDK_Api\SDKlib;DYV_SDK_Api\SDKlib;ViduSdk\SDKlib;ViduSdk_Internal\SDKlib;..\build\lib;OkuloSdk-windows\SDKlib;OkuloSdk\SDKlib;OkuloSdk_Internal\SDKlib' 'PATH' '%USE_ADMIN_ENV%'"
Powershell.exe -executionpolicy Bypass -Command "& '%cd%\camdriver\set_env_path.ps1' '%SDK_LIB_PATH%\SDKpythonLib' 'ViduSdk-;ViduSdk_Internal-;Vidu_SDK_Api\SDKlib\SDKpythonLib;ViduSdk\SDKlib\SDKpythonLib;ViduSdk_Internal\SDKlib\SDKpythonLib;..\build\lib' 'PYTHONPATH' '%USE_ADMIN_ENV%'"
SET "script_path=%cd%\camdriver\okulo_c1_metadata_win.ps1"
Powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& {Start-Process Powershell.exe -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File ""%script_path%""' -Verb RunAs}"
echo "finish, need to reopen the console window!"
