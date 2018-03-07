LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ndk_pthread
LOCAL_SRC_FILES := ndk_pthread.c
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
