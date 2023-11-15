@echo off
SET "SDK_LIB_PATH=%cd%\SDKlib"
setx DYVCAM_GENTL64_PATH "%SDK_LIB_PATH%"
Powershell.exe -executionpolicy Bypass -Command "& '%cd%\camdriver\setEnvPath.ps1' '%SDK_LIB_PATH%' 'Vidu_SDK_Api\SDKlib;DYV_SDK_Api\SDKlib;ViduSdk\SDKlib;ViduSdk_Internal\SDKlib;DYV_SDK\build;OkuloSdk-windows\SDKlib;OkuloSdk\SDKlib;OkuloSdk_Internal\SDKlib' 'PATH' '%USE_ADMIN_ENV%'"
Powershell.exe -executionpolicy Bypass -Command "& '%cd%\camdriver\setEnvPath.ps1' '%SDK_LIB_PATH%\SDKpythonLib' 'Vidu_SDK_Api\SDKlib\SDKpythonLib;ViduSdk\SDKlib\SDKpythonLib;ViduSdk_Internal\SDKlib\SDKpythonLib;DYV_SDK\build' 'PYTHONPATH' '%USE_ADMIN_ENV%'"
echo "finish, need to reopen the console window!"
