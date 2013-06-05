LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE                        := vorbis
LOCAL_SRC_FILES                     := ../../lib/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE                        := ogg
LOCAL_SRC_FILES                     := ../../lib/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -std=c++11 -D_LITTLE_ENDIAN=1 -DBOOST_EXCEPTION_DISABLE -D_STLP_NO_EXCEPTIONS -DOS_ANDROID -D_STLP_USE_SIMPLE_NODE_ALLOC
LOCAL_C_INCLUDES := ../../../../boost-libs/include ../../src
LOCAL_MODULE    := jngl-test

LOCAL_SRC_FILES := main.cpp
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -ljngl -L../jngl/obj/local/armeabi -L../lib -lfreetype -lpng -lz -logg -lvorbis -lopenal
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -std=c++11 -D_LITTLE_ENDIAN=1 -D_STLP_NO_EXCEPTIONS -DOS_ANDROID -D_STLP_USE_SIMPLE_NODE_ALLOC
LOCAL_MODULE    := bootstrap

LOCAL_SRC_FILES := bootstrap.cpp
LOCAL_LDLIBS    := -llog -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_CPP_FEATURES += exceptions rtti

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)