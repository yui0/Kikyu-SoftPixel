# NDK Makefile

NDK_TOOLCHAIN_ROOT=/android-toolchain
LOCAL_PATH := $(call my-dir)

#
## libNative.so
#
include $(CLEAR_VARS)
LOCAL_MODULE	:= Native
LOCAL_SRC_FILES	:= android_native_app_glue.c
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -I$(LOCAL_PATH)/include
LOCAL_LDLIBS := -landroid -llog
include $(BUILD_SHARED_LIBRARY)
