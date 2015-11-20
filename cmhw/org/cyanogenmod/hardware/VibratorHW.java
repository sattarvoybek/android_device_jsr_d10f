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

import org.cyanogenmod.hardware.util.FileUtils;
import java.io.File;

public class VibratorHW {

    private static String VTG_MIN_PATH = "/sys/class/timed_output/vibrator/vtg_min";
    private static String VTG_MAX_PATH = "/sys/class/timed_output/vibrator/vtg_max";
    private static String VTG_LEVEL_PATH = "/sys/class/timed_output/vibrator/vtg_level";

    public static boolean isSupported() {
        return new File(VTG_LEVEL_PATH).exists();
    }

    public static int getMaxIntensity()  {
        return Integer.parseInt(FileUtils.readOneLine(VTG_MAX_PATH));
    }
    public static int getMinIntensity()  {
        return Integer.parseInt(FileUtils.readOneLine(VTG_MIN_PATH));
    }
    public static int getWarningThreshold()  {
        return -1;
    }
    public static int getCurIntensity()  {
        return Integer.parseInt(FileUtils.readOneLine(VTG_LEVEL_PATH));
    }
    public static int getDefaultIntensity()  {
        return Integer.parseInt(FileUtils.readOneLine(VTG_MAX_PATH));
    }
    public static boolean setIntensity(int intensity)  {
        return FileUtils.writeLine(VTG_LEVEL_PATH, String.valueOf(intensity));
    }
}
