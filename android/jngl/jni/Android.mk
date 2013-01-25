LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -std=c++11 -DOPENGLES -D_LITTLE_ENDIAN=1
LOCAL_C_INCLUDES := ../../../boost-libs/include
LOCAL_MODULE := jngl
LOCAL_SRC_FILES := ../../../src/main.cpp
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_STATIC_LIBRARY)