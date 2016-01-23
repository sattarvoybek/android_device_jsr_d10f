LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Ofast
LOCAL_SRC_FILES += \
    ../../../../packages/apps/OpenDelta/jni/zipadjust.c\
    ../../../../packages/apps/OpenDelta/jni/zipadjust_run.c

LOCAL_LDLIBS := -lz

LOCAL_C_INCLUDES += external/zlib

ifeq ($(HOST_OS),linux)
LOCAL_LDLIBS += -lrt
endif

ifneq ($(strip $(USE_MINGW)),)
LOCAL_STATIC_LIBRARIES += libz
else
LOCAL_LDLIBS += -lz
endif

ifneq ($(strip $(BUILD_HOST_static)),)
LOCAL_LDLIBS += -lpthread
endif # BUILD_HOST_static


LOCAL_MODULE := zipadjust

include $(BUILD_HOST_EXECUTABLE)
