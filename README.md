# JNGL - Just a Neat Game Library

[🌐 Website](https://bixense.com/jngl/)
[🏃 Quickstart](https://github.com/jhasse/jngl/wiki/JNGL-Quickstart)
[🎓 Tutorial](https://github.com/pinguin999/my-jngl-starter)
[📚 Documentation](https://bixense.com/jngl/annotated.html)

[![Chat](https://img.shields.io/badge/chat-on%20discord-7289da.svg)](https://discord.gg/254P43HvWJ)

An easy to use C++ game library for Linux, Windows, macOS, Android, iOS, Xbox, the Nintendo Switch,
LG webOS and the Web.

[![Portal Dogs running on the Switch](https://user-images.githubusercontent.com/80071/105062511-f651d480-5a7a-11eb-8cd3-260c7929353a.gif)](https://portaldogs.com/)

### Building from Source
```bash
cmake -Bbuild
cmake --build build
./build/jngl-test
```

## Linux
### Ubuntu
```bash
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libpng-dev \
libxxf86vm-dev libvorbis-dev cmake g++ libwebp-dev git libsdl3-dev
```
### Fedora
```bash
sudo dnf install fontconfig-devel freetype-devel libvorbis-devel libwebp-devel cmake \
ninja-build SDL3-devel gcc-c++ libatomic libavformat-free-devel libswscale-free-devel
```
The last two are only needed if you plan to use [VideoRecorder](https://bixense.com/jngl/classjngl_1_1VideoRecorder.html#details).
### Arch Linux
```bash
pacman -Syu --needed cmake gcc sdl3 pkg-config fontconfig libwebp libvorbis
```

## Windows
### MSYS2 / MinGW-w64
Set up [MSYS2](https://www.msys2.org/) and install the following in a MinGW-w64 Win64 Shell:
```bash
pacman -Syu --needed mingw-w64-x86_64-gcc \
mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libwebp \
mingw-w64-x86_64-dlfcn mingw-w64-x86_64-cmake make mingw-w64-x86_64-gdb \
mingw-w64-x86_64-libtheora mingw-w64-x86_64-SDL3
```
### Visual Studio 2017 or newer
```bash
cmake -Bbuild -DFETCHCONTENT_QUIET=0
```
Then open `build/jngl.sln` in Visual Studio.

### Vulkan
Install the Vulkan SDK, including shaderc, and open a new terminal so that `VULKAN_SDK` and
`PATH` include it. Enable the experimental Vulkan renderer with the default SDL3 window backend:
```bash
cmake -Bbuild-vulkan -DJNGL_VULKAN=1 -DJNGL_WINAPI=OFF
cmake --build build-vulkan --config Release
```
The same option works with cja: `cja build -DJNGL_VULKAN=1`.

At runtime, `shaderc_shared.dll` from the SDK's `Bin` directory must be on `PATH` or beside the
executable. Include this DLL when distributing a Vulkan build. The Vulkan loader is supplied by
the graphics driver; users need a Vulkan-capable driver, but do not need the SDK.

### Direct3D 12
The experimental Direct3D 12 renderer also needs the Vulkan SDK to build, for its shader compilers
(GLSL is compiled to SPIR-V and translated to HLSL by SPIRV-Cross):
```bash
cmake -Bbuild-d3d12 -DJNGL_D3D12=1 -DJNGL_WINAPI=OFF
cmake --build build-d3d12 --config Release
```
At runtime, `shaderc_shared.dll` and `spirv-cross-c-shared.dll` from the SDK's `Bin` directory
must be on `PATH` or beside the executable. Include these DLLs when distributing a Direct3D 12
build. Debug builds enable the Direct3D 12 debug layer if the Graphics Tools optional feature of
Windows is installed.

## macOS
Use [Homebrew](http://brew.sh/) to install the build dependencies:
```bash
brew install sdl3 freetype libvorbis webp pkg-config cmake ffmpeg
```

`ffmpeg` is only needed if you plan to use [VideoRecorder](https://bixense.com/jngl/classjngl_1_1VideoRecorder.html#details).

### Vulkan
On Apple Silicon with macOS 26 or newer, Homebrew's Mesa package includes the KosmicKrisp
Vulkan driver, which runs on Metal. Install the Vulkan dependencies and select that driver:
```bash
brew install mesa vulkan-loader vulkan-headers shaderc vulkan-tools vulkan-validationlayers
export VK_DRIVER_FILES="$(brew --prefix mesa)/share/vulkan/icd.d/kosmickrisp_mesa_icd.aarch64.json"
vulkaninfo --summary
cmake -Bbuild-vulkan -DJNGL_VULKAN=ON
cmake --build build-vulkan
./build-vulkan/jngl-test
```

To enable Homebrew's validation layer in Debug builds, also set:
```bash
export VK_ADD_LAYER_PATH="$(brew --prefix)/share/vulkan/explicit_layer.d"
export DYLD_LIBRARY_PATH="$(brew --prefix vulkan-validationlayers)/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
```

For Intel Macs or older macOS versions, use `brew install molten-vk` and select its driver with
`export VK_DRIVER_FILES="$(brew --prefix)/etc/vulkan/icd.d/MoltenVK_icd.json"` instead.
MoltenVK currently hits sampler limits in the descriptor-pool stress test; prefer KosmicKrisp
where available.
The LunarG Vulkan SDK can also supply the dependencies; source its `setup-env.sh` before
configuring. JNGL links to the Vulkan loader, so the driver can be selected at runtime.
When distributing a Vulkan build, bundle the loader and driver with the application.

## Android
1. Install the Android SDK and set `ANDROID_HOME` to point to it.
2. In the Android SDK Manager, install the NDK build tools.
3. Run the following command to test via ADB:
```bash
lus android-run
```
Alternatively, open `android/test` in Android Studio and build from there.

## iOS
Generate an Xcode project using CMake:
```bash
cmake -Bbuild-ios -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/iOS.toolchain.cmake -DIOS_PLATFORM=SIMULATOR
```
Then open and build `build-ios/jngl.xcodeproj` in Xcode.

## Example Usage
Here's a simple "Hello, World!" example in JNGL:
```cpp
#include <jngl.hpp>
#include <jngl/init.hpp>

class MyGame : public jngl::Scene {
    void step() override {
        // game logic
    }
    void draw() const override {
        text.draw(jngl::modelview().translate({ -100, 0 }));
    }
    jngl::Font font{ "Arial.ttf", 12 };
    jngl::TextLine text{ font, "Hello World!" };
};

jngl::AppParameters jnglInit() {
    jngl::AppParameters params;
    params.start = []() {
        return std::make_shared<MyGame>();
    };
    return params;
}
```

For a more complete starting point (i.e. project structure, etc.) check out the
[JNGL project template](https://github.com/jhasse/jngl-starter).
