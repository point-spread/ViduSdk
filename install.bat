@REM @echo off
echo %~dp0/../thirdParty/opencv/opencv/build/x64/vc15/bin
setx PATH "%PATH%;%~dp0SDKlib"
setx DYVCAM_GENTL64_PATH "%~dp0SDKlib"