project(jngl-test)
cmake_minimum_required(VERSION 3.16)

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(jngl-test SHARED main.cpp)

target_include_directories(jngl-test PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ../../../../../../src
    ../../../../../include)

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../../../.. ${CMAKE_CURRENT_BINARY_DIR}/jngl)

# add lib dependencies
target_link_libraries(jngl-test
    android
    native_app_glue
    EGL
    GLESv3
    OpenSLES
    z
    jngl
)


add_library(bootstrap SHARED bootstrap.cpp)

target_include_directories(bootstrap PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
target_link_libraries(bootstrap
    android
    native_app_glue
    log)
