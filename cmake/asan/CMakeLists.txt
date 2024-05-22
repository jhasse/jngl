cmake_minimum_required(VERSION 3.6)
project(STATIC_ASAN LANGUAGES CXX)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "int main() {}")
add_executable(foo main.cpp)

target_compile_options(foo PRIVATE -fno-omit-frame-pointer -fsanitize=address -static-libasan)
target_link_options(foo PRIVATE -fno-omit-frame-pointer -fsanitize=address -static-libasan)
