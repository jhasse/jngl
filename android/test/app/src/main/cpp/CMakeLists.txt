#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

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
    GLESv1_CM
    OpenSLES
    log
    z
    jngl
    ${PROJECT_SOURCE_DIR}/../../../../../lib/${ANDROID_ABI}/libfreetype.a
)


add_library(bootstrap SHARED bootstrap.cpp)

target_include_directories(bootstrap PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
target_link_libraries(bootstrap
    android
    native_app_glue
    log)
