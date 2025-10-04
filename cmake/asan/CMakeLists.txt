cmake_minimum_required(VERSION 3.10)
project(STATIC_ASAN LANGUAGES CXX)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "int main() {}")
add_executable(foo main.cpp)

if(MSVC)
	set(_dont_ignore_unknown_flags "/WX")
endif()
target_compile_options(foo PRIVATE -fno-omit-frame-pointer -fsanitize=address -static-libasan ${_dont_ignore_unknown_flags})
target_link_options(foo PRIVATE -fno-omit-frame-pointer -fsanitize=address -static-libasan ${_dont_ignore_unknown_flags})
