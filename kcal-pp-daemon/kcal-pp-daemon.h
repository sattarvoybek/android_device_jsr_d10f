/*
 * Copyright (C) 2015 S-trace <S-trace@list.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

// The truth is out there
#define FALSE (0)
#define TRUE !FALSE

// Logcat assist
#define FNLOG()             ALOGV("%s", __FUNCTION__);
#define DEBUG(fmt, ...)     ALOGV("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define INFO(fmt, ...)      ALOGI("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define ERROR(fmt, ...)     ALOGE("%s:%d " fmt,__FUNCTION__,__LINE__, ## __VA_ARGS__)

// KCAL limits
#define KCAL_MAX_HUE 1536
#define KCAL_MIN_HUE 0
#define KCAL_DEF_HUE 0

#define KCAL_MAX_SAT 383
#define KCAL_MIN_SAT 224
#define KCAL_DEF_SAT 255

#define KCAL_MAX_INT 383
#define KCAL_MIN_INT 224
#define KCAL_DEF_INT 255

#define KCAL_MAX_CNT 383
#define KCAL_MIN_CNT 128
#define KCAL_DEF_CNT 255

// KCAL sysfs files
#define KCAL_PATH "/sys/devices/platform/kcal_ctrl.0/"
#define KCAL_HUE_FILE "kcal_hue"
#define KCAL_SAT_FILE "kcal_sat"
#define KCAL_INT_FILE "kcal_val"
#define KCAL_CNT_FILE "kcal_cont"
#define KCAL_ENABLE_FILE "kcal_enable"

// Socket name
#define CONTROL_SOCKET_NAME "pps"

// Socket requests
#define REQUEST_PP_ON  "pp:on"
#define REQUEST_PP_OFF "pp:off"
#define REQUEST_STATUS "pp:query:status:postproc"
#define REQUEST_SET    "pp:set:hsic"
#define REQUEST_FORMAT "pp:set:hsic %d %lf %d %lf" // Used to parse request
#define REQUEST_RESET  "pp:set:hsic 0 0.0 0 0.0"   // Not a socket request, but just helper

// Socket replys
#define REPLY_RUNNING "Running\n"
#define REPLY_STOPPED "Stopped\n"
#define REPLY_SUCCESS "Success\n"
#define REPLY_FAILURE "Failure\n"

// Props used to save configuration
#define PROP_PP_ENABLED    "persist.kcal_pp.enabled"
#define PROP_PP_HUE        "persist.kcal_pp.hue"
#define PROP_PP_SATURATION "persist.kcal_pp.saturation"
#define PROP_PP_INTENSITY  "persist.kcal_pp.intensity"
#define PROP_PP_CONTRAST   "persist.kcal_pp.contrast"
