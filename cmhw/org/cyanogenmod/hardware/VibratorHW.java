/*
 * Copyright (C) 2013 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cyanogenmod.hardware;

import android.util.Log;
import org.cyanogenmod.hardware.util.FileUtils;
import java.io.File;
import java.util.Scanner;

public class VibratorHW {

    private static String VTG_MIN_PATH = "/sys/class/timed_output/vibrator/vtg_min";
    private static String VTG_MAX_PATH = "/sys/class/timed_output/vibrator/vtg_max";
    private static String VTG_LEVEL_PATH = "/sys/class/timed_output/vibrator/vtg_level";

    public static boolean isSupported() {
        return new File(VTG_LEVEL_PATH).exists();
    }

    public static int getMaxIntensity()  {
        int ret = 31;  // 31 is a default max value in kernel
        try {
            Scanner s = new Scanner(new File(VTG_MAX_PATH));
            ret = s.nextInt();
            s.close();
        } catch (Exception ex) {
	    Log.e("VibratorHW", "getMaxIntensity failed with " + ex);
	}
	return ret;
    }
    public static int getMinIntensity()  {
        int ret = 12;  // 12 is a default min value in kernel
        try {
            Scanner s = new Scanner(new File(VTG_MIN_PATH));
            ret = s.nextInt();
            s.close();
        } catch (Exception ex) {
	    Log.e("VibratorHW", "getMinIntensity failed with " + ex);
	}
	return ret;
    }
    public static int getWarningThreshold()  {
        return -1;
    }
    public static int getCurIntensity()  {
	int ret = getMaxIntensity();
        try {
            Scanner s = new Scanner(new File(VTG_LEVEL_PATH));
            ret = s.nextInt();
            s.close();
        } catch (Exception ex) {
	    Log.e("VibratorHW", "getCurIntensity failed with " + ex);
	}
	return ret;
    }
    public static int getDefaultIntensity()  {
        return getMaxIntensity();
    }
    public static boolean setIntensity(int intensity)  {
	boolean ret = false;
        try {
	    FileUtils.writeLine(VTG_LEVEL_PATH, String.valueOf(intensity));
	    ret = true;
        } catch (Exception ex) {
	    Log.e("VibratorHW", "setIntensity failed with " + ex);
	}
	return ret;
    }
}
