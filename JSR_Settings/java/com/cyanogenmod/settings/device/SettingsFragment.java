package com.cyanogenmod.settings.device;

import android.content.Intent;
import android.content.ComponentName;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.SwitchPreference;
import android.preference.ListPreference;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.provider.Settings;
import android.widget.Toast;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.Arrays;

/**
 * Created by prodoomman on 19.02.15.
 */
public class SettingsFragment extends PreferenceFragment implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "JSR";
    private static final String BTN_FUNC_APP = "btn_func_app";
    private static final String BTN_FUNC_APP2 = "btn_func_app2";
    private static final String PERSISTENT_PROPERTY_CONFIGURATION_NAME = "persist.storages.configuration";
    private static final int STORAGES_CONFIGURATION_CLASSIC = 0 ;
    private static final int STORAGES_CONFIGURATION_INVERTED = 1 ;
    private static final int STORAGES_CONFIGURATION_DATAMEDIA = 2 ;

    EditTextPreferenceEx btn_func_app;
    EditTextPreferenceEx btn_func_app2;
    Preference btn_g_cal;

    private class SysfsValue {
        private String fileName;
        private String value;

        private SysfsValue(String fileName, String value) {
            this.fileName = fileName;
            this.value = value;
        }

        public String getFileName() {
            return fileName;
        }

        public String getValue() {
            return value;
        }
    }

    class SysfsWriteTask extends AsyncTask<SysfsValue, Void, Integer> {

        @Override
        protected Integer doInBackground(SysfsValue... params) {
            try {
                FileOutputStream fos = new FileOutputStream(params[0].getFileName());
                fos.write(params[0].getValue().getBytes(Charset.forName("UTF-8")));
                fos.close();
            } catch (Exception e) {
                e.printStackTrace();
                return -1;
            }
            return 0;
        }

        @Override
        protected void onPostExecute(Integer result) {
            if(0 != result) {
                Toast.makeText(getActivity(), R.string.fail, Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.pref_jsr);

        ListPreference main_storage = (ListPreference)findPreference("main_storage");
        main_storage.setOnPreferenceChangeListener(this);

        int configuration = SystemProperties.getInt(PERSISTENT_PROPERTY_CONFIGURATION_NAME, STORAGES_CONFIGURATION_CLASSIC);

        main_storage.setValue(String.valueOf(configuration));

        btn_func_app = (EditTextPreferenceEx)findPreference(BTN_FUNC_APP);
        btn_func_app.setOnPreferenceChangeListener(this);

        btn_func_app2 = (EditTextPreferenceEx)findPreference(BTN_FUNC_APP2);
        btn_func_app2.setOnPreferenceChangeListener(this);

        btn_g_cal = (Preference)findPreference("btn_g_cal");
        btn_g_cal.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener()
        {
            @Override
            public boolean onPreferenceClick(Preference pref) {
                if (pref.getKey().equals("btn_g_cal")) {
                    try {
                        Intent callGCal = new Intent();
                        callGCal.setComponent(new ComponentName("com.qualcomm.sensors.qsensortest",
                                "com.qualcomm.sensors.qsensortest.TabControl"));
                        startActivity(callGCal);
                    }
                    catch(Exception ex) {
                        Log.e("JSR_settings", "Failed to start GravityCalibrationActivity: " + ex);
                        Toast.makeText(getActivity(), R.string.btn_g_cal_failure, Toast.LENGTH_LONG).show();
                    }
                }
                return true;
            }
        });
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue)
    {
        if (preference.getKey().equals("main_storage")) {
            SystemProperties.set(PERSISTENT_PROPERTY_CONFIGURATION_NAME, (String)newValue);
            Toast.makeText(getActivity(), R.string.reboot_needed, Toast.LENGTH_LONG).show();
        }
        if (preference.getKey().equals(BTN_FUNC_APP)) {
            Settings.System.putString(getActivity().getContentResolver(), preference.getKey(), (String)newValue);
        }
        if (preference.getKey().equals(BTN_FUNC_APP2)) {
            Settings.System.putString(getActivity().getContentResolver(), preference.getKey(), (String)newValue);
        }
        return true;
    }
}
