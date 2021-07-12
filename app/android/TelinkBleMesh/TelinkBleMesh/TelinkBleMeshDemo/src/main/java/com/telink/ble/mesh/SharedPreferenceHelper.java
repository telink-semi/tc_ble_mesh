/********************************************************************************************************
 * @file SharedPreferenceHelper.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh;

import android.content.Context;
import android.content.SharedPreferences;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.util.Arrays;


/**
 * Created by kee on 2017/8/30.
 */

public class SharedPreferenceHelper {

    private static final String DEFAULT_NAME = "telink_shared";
    private static final String KEY_FIRST_LOAD = "com.telink.bluetooth.light.KEY_FIRST_LOAD";

    private static final String KEY_LOCATION_IGNORE = "com.telink.bluetooth.light.KEY_LOCATION_IGNORE";

    private static final String KEY_LOG_ENABLE = "com.telink.bluetooth.light.KEY_LOG_ENABLE";

    /**
     * scan device by private mode
     */
    private static final String KEY_PRIVATE_MODE = "com.telink.bluetooth.light.KEY_PRIVATE_MODE";

    private static final String KEY_LOCAL_UUID = "com.telink.bluetooth.light.KEY_LOCAL_UUID";

    private static final String KEY_REMOTE_PROVISION = "com.telink.bluetooth.light.KEY_REMOTE_PROVISION";

    private static final String KEY_FAST_PROVISION = "com.telink.bluetooth.light.KEY_FAST_PROVISION";

    private static final String KEY_NO_OOB = "com.telink.bluetooth.light.KEY_NO_OOB";

    private static final String KEY_EXTEND_BEARER_MODE = "com.telink.bluetooth.light.KEY_EXTEND_BEARER_MODE";

    private static final String KEY_AUTO_PV = "com.telink.bluetooth.light.KEY_AUTO_PV";

    private static final String KEY_DIST_ADR = "com.telink.bluetooth.light.KEY_DIST_ADR";

    private static final String KEY_APPLY_POLICY = "com.telink.bluetooth.light.KEY_APPLY_POLICY";

    public static boolean isFirstLoad(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_FIRST_LOAD, true);
    }

    public static void setFirst(Context context, boolean isFirst) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_FIRST_LOAD, isFirst).apply();
    }


    public static boolean isLocationIgnore(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_LOCATION_IGNORE, false);
    }

    public static void setLocationIgnore(Context context, boolean ignore) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LOCATION_IGNORE, ignore).apply();
    }

    public static boolean isLogEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_LOG_ENABLE, false);
    }

    public static void setLogEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LOG_ENABLE, enable).apply();
    }

    public static boolean isPrivateMode(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_PRIVATE_MODE, false);
    }

    public static void setPrivateMode(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_PRIVATE_MODE, enable).apply();
    }

    public static String getLocalUUID(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        String uuid = sharedPreferences.getString(KEY_LOCAL_UUID, null);
        if (uuid == null) {
            uuid = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
            sharedPreferences.edit().putString(KEY_LOCAL_UUID, uuid).apply();
        }
        return uuid;

    }


    public static boolean isRemoteProvisionEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_REMOTE_PROVISION, false);
    }

    public static void setRemoteProvisionEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_REMOTE_PROVISION, enable).apply();
    }


    public static boolean isFastProvisionEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_FAST_PROVISION, false);
    }

    public static void setFastProvisionEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_FAST_PROVISION, enable).apply();
    }


    public static boolean isNoOOBEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_NO_OOB, true);
    }

    public static void setNoOOBEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_NO_OOB, enable).apply();
    }


    public static ExtendBearerMode getExtendBearerMode(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return ExtendBearerMode.valueOf(sharedPreferences.getString(KEY_EXTEND_BEARER_MODE, ExtendBearerMode.NONE.name()));
    }

    public static void setExtendBearerMode(Context context, ExtendBearerMode extendBearerMode) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putString(KEY_EXTEND_BEARER_MODE, extendBearerMode.name()).apply();
    }

    public static boolean isAutoPvEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_AUTO_PV, false);
    }

    public static void setAutoPvEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_AUTO_PV, enable).apply();
    }


    /*public static void setMeshOTAState(Context context, int distAddress, UpdatePolicy policy) {
        MeshLogger.d("save meshOTA state: distAdr - " + distAddress + " -- " + policy);
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_DIST_ADR, distAddress)
                .putString(KEY_APPLY_POLICY, policy.name())
                .apply();
    }

    public static void clearMeshOTAState(Context context) {
        MeshLogger.d("clear meshOTA state -- ");
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_DIST_ADR, 0)
                .putString(KEY_APPLY_POLICY, "")
                .apply();
    }*/


}
