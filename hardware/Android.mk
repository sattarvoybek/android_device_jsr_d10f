TARGET_HARDWARE_PATH := $(call my-dir)

ifneq ($(QCPATH),)
ifeq ($(TARGET_DEVICE),d10f)

ifeq ($(TARGET_PROVIDES_GPS_LOC_API),true)
include $(TARGET_HARDWARE_PATH)/gps/Android.mk
endif

endif
endif