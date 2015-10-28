cd build
if grep -q "UTC%z" tools/buildinfo.sh
then
    echo '[build] buildinfo.sh already patched';
else
    git am ../device/jsr/d10f/patches/build/0001-Change-date-format.patch || git am --abort
fi
if grep -q "post_process_props_hook" tools/post_process_props.py
then
    echo '[build] post_process_props.py already patched';
else
    git am ../device/jsr/d10f/patches/build/0002-Add-hook-functions-to-post_process_props.py.patch || git am --abort
fi
if grep -q -- "--block" core/Makefile
then
    git am ../device/jsr/d10f/patches/build/0003-Disable-block-zip-building-build-standard-file-based.patch || git am --abort
else
    echo '[build] post_process_props.py already patched';
fi
croot

cd frameworks/base
if grep -q "ro.storage_list.override" services/core/java/com/android/server/MountService.java
then
    echo '[storages] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks/base/0001-MountService-allow-overriding-default-storage-list-w.patch || git am --abort
    git am ../../device/jsr/d10f/patches/frameworks/base/0002-frameworks-hardcode-package-name-to-android.patch || git am --abort
fi
if grep -q "enabling torch via sysfs" services/core/java/com/android/server/TorchService.java
then
    echo '[Torch] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks/base/0003-TORCH-Use-sysfs-interface-for-system-wide-torch-serv.patch || git am --abort
fi
if grep -q "/sys/class/power_supply/battery/charge_full_design" core/java/com/android/internal/os/PowerProfile.java
then
    echo '[Battery] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks/base/0004-POWER-Read-battery-capacity-from-sysfs.patch || git am --abort
fi
if grep -q "DEFAULT_THRESHOLD_MAX_BYTES = 10" core/java/android/os/storage/StorageManager.java; then
    echo '[storages] DEFAULT_THRESHOLD_MAX_BYTES in Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks/base/0005-STORAGES-Reduce-DEFAULT_THRESHOLD_MAX_BYTES-500MB-10.patch || git am --abort
fi
croot

cd frameworks/opt/net/wifi
if grep -q "disabling Wifi AP due to Subscription change" service/java/com/android/server/wifi/WifiController.java
then
    git am ../../../../device/jsr/d10f/patches/frameworks/opt/net/wifi/0001-Revert-wifi-disable-access-point-on-subscription-cha.patch || git am --abort
else
    echo '[WLAN] frameworks/opt/net/wifi already patched';
fi
croot

cd kernel/jsr/msm8226
if grep -q "VENUS_EXTRADATA_SIZE" include/media/msm_media_info.h
then
    echo '[kernel] include/media/msm_media_info.h already patched';
else
    git am ../../../device/jsr/d10f/patches/kernel/jsr/msm8226/0001-msm-vidc-Amend-calculation-of-buffer-sizes-in-VENUS_.patch || git am --abort
    git am ../../../device/jsr/d10f/patches/kernel/jsr/msm8226/0002-msm-vidc-Expose-extradata-size-to-userspace.patch || git am --abort
fi
croot

cd bionic
if grep -q "tzdata2015g" libc/zoneinfo/tzdata
then
    echo '[bionic] tzdata already patched';
else
    git am ../device/jsr/d10f/patches/bionic/0001-TZDATA-Upgrade-tzdata-to-2015g.patch || git am --abort
fi
croot

cd external/icu
if grep -q "2015g" icu4c/source/data/misc/zoneinfo64.txt
then
    echo '[icu] zoneinfo already patched';
else
    git am ../../device/jsr/d10f/patches/external/icu/0001-TZDATA-Update-tzdata-to-2015g.patch || git am --abort
fi
croot

cd packages/apps/Torch
if grep -q android.hardware.ITorchService src/net/cactii/flash2/FlashDevice.java ; then
    git am ../../../device/jsr/d10f/patches/packages/apps/Torch/0001-Revert-Torch-notify-TorchService-of-torch-state-when.patch || git am --abort
    git am ../../../device/jsr/d10f/patches/packages/apps/Torch/0002-Revert-Make-torch-shutdown-by-camera-usage-work-prop.patch || git am --abort
    git am ../../../device/jsr/d10f/patches/packages/apps/Torch/0003-Revert-Torch-signal-to-framework-TorchService-not-to.patch || git am --abort
else
    echo '[torch] src/net/cactii/flash2/FlashDevice.java already patched';
fi
croot

cd system/vold
if grep -q "exfat.ko" Exfat.cpp
then
    echo '[Vold] Exfat kernel driver support already patched';
else
    git am ../../device/jsr/d10f/patches/system/vold/0001-EXFAT-Use-kernel-driver-instead-of-FUSE-driver.patch || git am --abort
fi
croot

sh device/jsr/d10f/update-overlay.sh
rm -f out/target/product/d10f/system/build.prop

add_lunch_combo cm_d10f-eng
add_lunch_combo cm_d10f-user
add_lunch_combo cm_d10f-userdebug
