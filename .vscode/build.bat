@echo off

:: see https://blog.pcitron.fr/2022/01/04/dont-use-vcvarsall-vsdevcmd/
set VSCMD_ARG_HOST_ARCH=x64
set VSCMD_ARG_TGT_ARCH=x64
set VSCMD_ARG_APP_PLAT=Desktop
set VSINSTALLDIR=C:\Program Files\Microsoft Visual Studio\2022\Community\
call "%VSINSTALLDIR%\Common7\Tools\vsdevcmd\core\winsdk.bat"
call "%VSINSTALLDIR%\Common7\Tools\vsdevcmd\ext\cmake.bat"
call "%VSINSTALLDIR%\Common7\Tools\vsdevcmd\ext\vcvars.bat"
if not defined INCLUDE set INCLUDE=%__VSCMD_VCVARS_INCLUDE%%__VSCMD_WINSDK_INCLUDE%%__VSCMD_NETFX_INCLUDE%%INCLUDE%

if exist build\build.ninja (
    rem file exists
) else (
    cmake -Bbuild -G"Ninja Multi-Config" -DJNGL_VIDEO=0
)
cmake --build build
