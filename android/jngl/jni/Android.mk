LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CPPFLAGS := -std=c++11
LOCAL_CFLAGS := -DOPENGLES -DNOJPEG -DNOWEBP
LOCAL_MODULE := jngl
SRC_PATH := $(LOCAL_PATH)/../../../src/
FILE_LIST := $(wildcard $(SRC_PATH)*.cpp $(SRC_PATH)*.c $(SRC_PATH)jngl/*.cpp $(SRC_PATH)android/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
