@echo off

call .vscode\env.bat

if exist build\build.ninja (
    rem file exists
) else (
    cmake -Bbuild -G"Ninja Multi-Config" -DJNGL_VIDEO=0
)
cmake --build build
