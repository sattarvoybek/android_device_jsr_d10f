TARGET_HARDWARE_PATH := $(call my-dir)

ifneq ($(QCPATH),)
ifeq ($(TARGET_DEVICE),d10f)

ifeq ($(strip $(USE_DEVICE_SPECIFIC_CAMERA)),true)
include $(TARGET_HARDWARE_PATH)/camera/Android.mk
endif

ifeq ($(TARGET_PROVIDES_GPS_LOC_API),true)
include $(TARGET_HARDWARE_PATH)/gps/Android.mk
endif

endif
endif