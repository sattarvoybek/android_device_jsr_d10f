#!/usr/bin/env python

import os, sys, time

# Put the modifications that you need to make into the /system/build.prop into this
# function. The prop object has get(name) and put(name,value) methods.
def mangle_build_prop_hook(prop, overrides):
  """call mangle_build_prop_hook"""
  
  prop.put("ro.com.android.dateformat", "yyyy-MM-dd")
  
  ver_inc = prop.get("ro.build.version.incremental")

  prod_dev = prop.get("ro.product.device")
  outdir = "out/target/product/" + prod_dev + "/"
  
  cm_ver = prop.get("ro.cm.version")
  cm_ver_arr = cm_ver.split("-")
  build_type = prop.get("ro.build.type")
  time_utc = time.gmtime(float(prop.get("ro.build.date.utc")))
  time_str = time.strftime("%Y%m%d-%H%M", time_utc)
  rom_name = "cm-" + cm_ver_arr[0] + "-" + time_str + "-" + prod_dev
  prop.put("ro.ota.current_rom", rom_name)
  
  txt = open(outdir + "ro_ota_current_rom", "w")
  txt.write(rom_name)
  txt.close()

  out_zip = "cm-" + cm_ver + ".zip"
  txt = open(outdir + "rom_out_package", "w")
  txt.write(out_zip)
  txt.close()
  pass

# Put the modifications that you need to make into the /system/build.prop into this
# function. The prop object has get(name) and put(name,value) methods.
def mangle_default_prop_hook(prop):
  """call mangle_default_prop_hook"""
  pass
