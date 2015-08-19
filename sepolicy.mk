
# SELinux
BOARD_SEPOLICY_DIRS += \
    device/jsr/d10f/sepolicy

BOARD_SEPOLICY_UNION += \
    file.te \
    app.te \
    file_contexts

#ifneq ($(TARGET_BUILD_VARIANT),user)
#    BOARD_SEPOLICY_UNION += su.te
#endif
