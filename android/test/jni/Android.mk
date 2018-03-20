LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := vorbis
LOCAL_SRC_FILES := ../../lib/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := ogg
LOCAL_SRC_FILES := ../../lib/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jngl
LOCAL_SRC_FILES := ../../jngl/obj/local/armeabi/libjngl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := freetype
LOCAL_SRC_FILES := ../../lib/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := png
LOCAL_SRC_FILES := ../../lib/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := openal
LOCAL_SRC_FILES := ../../lib/libopenal.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../src
LOCAL_MODULE    := jngl-test

LOCAL_SRC_FILES := main.cpp
LOCAL_LDLIBS    += -llog -landroid -lEGL -lGLESv1_CM -lz
LOCAL_STATIC_LIBRARIES := android_native_app_glue jngl freetype png openal boost_system_static
LOCAL_SHARED_LIBRARIES := ogg vorbis
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -std=c++11
LOCAL_MODULE := bootstrap

LOCAL_SRC_FILES := bootstrap.cpp
LOCAL_LDLIBS    := -llog -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,boost/1.59.0)
