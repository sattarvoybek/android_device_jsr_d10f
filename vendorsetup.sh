add_lunch_combo cm_d10f-userdebug

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

cd frameworks/base
if grep -q "ro.storage_list.override" services/java/com/android/server/MountService.java
then
    echo '[storages] Frameworks/base already patched';
else
    git am ../../device/jsr/d10f/patches/frameworks-base-1.patch || git am --abort
    git am ../../device/jsr/d10f/patches/frameworks-base-2.patch || git am --abort
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

sh device/jsr/d10f/update-overlay.sh
