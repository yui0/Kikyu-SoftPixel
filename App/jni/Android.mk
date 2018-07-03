# NDK Makefile

NDK_TOOLCHAIN_ROOT=/android-toolchain
LOCAL_PATH := $(call my-dir)

#
## libSoftPixelEngine.so
#
include $(CLEAR_VARS)
LOCAL_MODULE    := libSoftPixelEngine
LOCAL_SRC_FILES := libSoftPixelEngine.so
include $(PREBUILT_SHARED_LIBRARY)

#
## libKikyu.so
#
include $(CLEAR_VARS)
LOCAL_MODULE	:= Kikyu
LOCAL_SRC_FILES	:= kikyu.cpp
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -I$(LOCAL_PATH)/include -I$(NDK_TOOLCHAIN_ROOT)/sysroot/usr/include/boost-1_49
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti
LOCAL_CPPFLAGS += -D_ANDROID
#LOCAL_LDLIBS := -lGLESv1_CM -ljnigraphics -llog
LOCAL_LDLIBS := -landroid -llog
##LOCAL_LDLIBS := -landroid -llog -L$(LOCAL_PATH) -lSoftPixelEngine
##LOCAL_LDLIBS := -landroid -llog $(NDK_TOOLCHAIN_ROOT)/sysroot/usr/lib/libboost_thread-gcc-mt-1_49.a $(NDK_TOOLCHAIN_ROOT)/sysroot/usr/lib/libboost_system-gcc-mt-1_49.a $(NDK_TOOLCHAIN_ROOT)/sysroot/usr/lib/libboost_filesystem-gcc-mt-1_49.a
LOCAL_SHARED_LIBRARIES := libSoftPixelEngine
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/native_app/Android.mk
