/bin/cp device/jsr/d10f/post_process_props_hook.py build/tools/
chmod  0775 build/tools/post_process_props_hook.py

cd build
if grep -q "post_process_props_hook" tools/post_process_props.py
then
    echo '[build] post_process_props.py already patched';
else
    git am ../device/jsr/d10f/patches/build-post_process_props.patch || git am --abort
fi
croot


add_lunch_combo cm_d10f-eng
add_lunch_combo cm_d10f-user
add_lunch_combo cm_d10f-userdebug

cd frameworks/base
if grep -q "ro.storage_list.override" services/java/com/android/server/MountService.java
then
    echo '[storages] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks-base-1.patch || git am --abort
    git am ../../device/jsr/d10f/patches/frameworks-base-2.patch || git am --abort
fi
croot

cd frameworks/base
if grep -q "/sys/class/power_supply/battery/charge_full_design" core/java/com/android/internal/os/PowerProfile.java
then
    echo '[Battery] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks-base-battery.patch || git am --abort
fi
croot

cd build
if grep -q "UTC%z" tools/buildinfo.sh
then
    echo '[build] buildinfo.sh already patched';
else
    git am ../device/jsr/d10f/patches/build-date-format-utc.patch || git am --abort
fi
croot

cd bionic
if grep -q "tzdata2015e" libc/zoneinfo/tzdata
then
    echo '[bionic] tzdata already patched';
else
    git am ../device/jsr/d10f/patches/bionic-tzdata.patch || git am --abort
fi
croot

cd packages/apps/Dialer
if grep -q "persist.call_recording.enabled" src/com/android/services/callrecorder/CallRecorderService.java
then
    echo '[Dialer] CallRecorderService.java already patched';
else
    git am ../../../device/jsr/d10f/patches/dialer-call-recording.patch || git am --abort
fi
croot

cd packages/services/Telephony
if grep -q "call_recording_format" res/xml/msim_call_feature_setting.xml
then
    echo '[Telephony] msim_call_feature_setting.xml already patched';
else
    git am ../../../device/jsr/d10f/patches/telephony-call_recording_format.patch || git am --abort
fi
croot

sh device/jsr/d10f/update-icu.sh
croot

sh device/jsr/d10f/update-overlay.sh
croot

#rm -f out/target/product/d10f/root/init.qcom.sdcard.rc
#rm -rf out/target/product/d10f/obj/ETC/init.qcom.sdcard.rc_intermediates
rm -rf out/target/product/d10f/obj/PACKAGING/target_files_intermediates
rm -f out/target/product/d10f/system/build.prop
rm -f out/target/product/d10f/root/default.prop
