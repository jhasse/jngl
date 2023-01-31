call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if exist build\build.ninja (
    rem file exists
) else (
    cmake -Bbuild -GNinja -DJNGL_VIDEO=0 -DCMAKE_BUILD_TYPE=Debug
)
cmake --build build
