/********************************************************************************************************
 * @file ContextUtil.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.util;

import android.content.Context;
import android.location.LocationManager;
import android.os.Build;
import android.util.DisplayMetrics;

import java.lang.reflect.Method;

import static android.os.Build.VERSION.SDK_INT;

/**
 * Operations with Android context
 * Created by Administrator on 2017/4/11.
 */
public class ContextUtil {
    public static final int SDK_VERSION = Build.VERSION.SDK_INT;

    /**
     * check is the phone location enabled
     *
     * @param context Context
     * @return check result
     */
    public static boolean isLocationEnable(final Context context) {
        LocationManager locationManager
                = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null) return false;
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        return gps || network;
    }

    /**
     * check sdk version >= android L
     *
     * @return check result
     */
    public static boolean versionAboveL() {
        return SDK_VERSION >= Build.VERSION_CODES.LOLLIPOP;
    }


    /**
     * check sdk version >= android N
     *
     * @return check result
     */
    public static boolean versionAboveN() {
        return SDK_VERSION >= Build.VERSION_CODES.N;
    }


    /**
     * check sdk version == android N
     *
     * @return check result
     */
    public static boolean versionIsN() {
        return SDK_VERSION == Build.VERSION_CODES.N;
    }

    /**
     * use reflect to skip warning
     */
    public static void skipReflectWarning() {
        if (SDK_INT < Build.VERSION_CODES.P) {
            return;
        }
        try {
            Method forName = Class.class.getDeclaredMethod("forName", String.class);
            Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
            Class<?> vmRuntimeClass = (Class<?>) forName.invoke(null, "dalvik.system.VMRuntime");
            Method getRuntime = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "getRuntime", null);
            Method setHiddenApiExemptions = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "setHiddenApiExemptions", new Class[]{String[].class});
            Object sVmRuntime = getRuntime.invoke(null);
            setHiddenApiExemptions.invoke(sVmRuntime, new Object[]{new String[]{"L"}});
        } catch (Throwable e) {
            e.printStackTrace();
            MeshLogger.e("reflect bootstrap failed:");
        }
    }


    public static int dpToPx(Context context, float dp) {
        DisplayMetrics metrics = context.getResources().getDisplayMetrics();
        return (int) (dp * (metrics.densityDpi / 160f));
    }
}
