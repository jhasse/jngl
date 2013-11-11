LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CFLAGS := -DOPENGLES -D_LITTLE_ENDIAN=1 -DNOJPEG -DNOWEBP
LOCAL_C_INCLUDES := ../../boost-libs/include
LOCAL_MODULE := jngl
SRC_PATH := $(LOCAL_PATH)/../../../src/
FILE_LIST := $(wildcard $(SRC_PATH)*.cpp $(SRC_PATH)*.c $(SRC_PATH)jngl/*.cpp $(SRC_PATH)android/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
NDK_TOOLCHAIN_VERSION=4.7

LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_STATIC_LIBRARY)
