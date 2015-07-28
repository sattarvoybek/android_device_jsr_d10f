add_lunch_combo jsr_d10f-eng
add_lunch_combo jsr_d10f-user
add_lunch_combo jsr_d10f-userdebug

cd frameworks/base
if grep -q "ro.storage_list.override" services/java/com/android/server/MountService.java
then
    echo '[storages] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks-base-1.patch || git am --abort
    git am ../../device/jsr/d10f/patches/frameworks-base-2.patch || git am --abort
fi

croot

#rm -f out/target/product/d10f/root/init.qcom.sdcard.rc
#rm -rf out/target/product/d10f/obj/ETC/init.qcom.sdcard.rc_intermediates
