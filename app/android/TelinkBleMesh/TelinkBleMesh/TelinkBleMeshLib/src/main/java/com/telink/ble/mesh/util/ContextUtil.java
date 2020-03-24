package com.telink.ble.mesh.util;

import android.content.Context;
import android.location.LocationManager;
import android.os.Build;

/**
 * Operations with Android context
 * Created by Administrator on 2017/4/11.
 */
public class ContextUtil {
    public static final int SDK_VERSION = Build.VERSION.SDK_INT;

    public static boolean isLocationEnable(final Context context) {
        LocationManager locationManager
                = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null) return false;
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        return gps || network;
    }


    public static boolean versionAboveL() {
        return SDK_VERSION >= Build.VERSION_CODES.LOLLIPOP;
    }

    public static boolean versionAboveN() {
        return SDK_VERSION >= Build.VERSION_CODES.N;
    }

    public static boolean versionIsN() {
        return SDK_VERSION == Build.VERSION_CODES.N;
    }
}
