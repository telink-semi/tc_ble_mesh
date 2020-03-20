package com.telink.ble.mesh.util;

import android.content.Context;
import android.location.LocationManager;
import android.os.Build;

/**
 * Operations with Android context
 * Created by Administrator on 2017/4/11.
 */
public class ContextUtil {
    public static boolean isLocationEnable(final Context context) {
        LocationManager locationManager
                = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        return gps || network;
    }

    public static boolean versionAboveN(){
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.N;
    }

    public static boolean versionIsN(){
        return Build.VERSION.SDK_INT == Build.VERSION_CODES.N;
    }

}
