ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

GPS_DIR_LIST :=

# add RPC dirs if RPC is available
ifneq ($(TARGET_NO_RPC),true)

GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api-rpc-50001/

endif #TARGET_NO_RPC

$(info ======== loc_api_v02 ==========) 

GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api_50001/
GPS_DIR_LIST += $(LOCAL_PATH)/loc_api_v02/
GPS_DIR_LIST += $(LOCAL_PATH)/ds_api/

#call the subfolders
include $(addsuffix Android.mk, $(GPS_DIR_LIST))

endif#BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
