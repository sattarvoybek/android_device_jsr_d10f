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
croot

sh device/jsr/d10f/update-overlay.sh
rm -f out/target/product/d10f/system/build.prop

add_lunch_combo cm_d10f-eng
add_lunch_combo cm_d10f-user
add_lunch_combo cm_d10f-userdebug
