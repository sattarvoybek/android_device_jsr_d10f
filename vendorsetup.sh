/bin/cp device/jsr/d10f/post_process_props_hook.py build/tools/
chmod  0775 build/tools/post_process_props_hook.py

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
if grep -vq -- "--block" core/Makefile
then
    echo '[build] post_process_props.py already patched';
else
    git am ../device/jsr/d10f/patches/build/0003-Disable-block-zip-building-build-standard-file-based.patch || git am --abort
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

# cd bionic
# if grep -q "tzdata2015f" libc/zoneinfo/tzdata
# then
#     echo '[bionic] tzdata already patched';
# else
#     git am ../device/jsr/d10f/patches/bionic/0001-PATCH-Update-tzdata-to-2015f.patch || git am --abort
# fi
# croot
# 
# sh device/jsr/d10f/update-icu.sh
# croot

sh device/jsr/d10f/update-overlay.sh
croot

#rm -f out/target/product/d10f/root/init.qcom.sdcard.rc
#rm -rf out/target/product/d10f/obj/ETC/init.qcom.sdcard.rc_intermediates
rm -rf out/target/product/d10f/obj/PACKAGING/target_files_intermediates
rm -f out/target/product/d10f/system/build.prop

add_lunch_combo cm_d10f-eng
add_lunch_combo cm_d10f-user
add_lunch_combo cm_d10f-userdebug
