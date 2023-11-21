call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if exist build\build.ninja (
    rem file exists
) else (
    cmake -Bbuild -G"Ninja Multi-Config" -DJNGL_VIDEO=0
)
cmake --build build
