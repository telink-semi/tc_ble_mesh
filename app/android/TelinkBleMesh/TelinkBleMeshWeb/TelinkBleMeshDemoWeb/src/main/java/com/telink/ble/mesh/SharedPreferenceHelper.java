/********************************************************************************************************
 * @file SharedPreferenceHelper.java
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
package com.telink.ble.mesh;

import android.content.Context;
import android.content.SharedPreferences;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.util.Arrays;

import java.util.UUID;


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


    private static final String KEY_LOGIN_NAME = "com.telink.bluetooth.light.KEY_LOGIN_NAME";

    private static final String KEY_LOGIN_PWD = "com.telink.bluetooth.light.KEY_LOGIN_PWD";

    private static final String KEY_NETWORK_ID = "com.telink.bluetooth.light.network_id_";

    private static final String KEY_CLIENT_ID = "com.telink.bluetooth.light.KEY_CLIENT_ID";

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
        return sharedPreferences.getBoolean(KEY_LOG_ENABLE, true);
    }

    public static void setLogEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LOG_ENABLE, enable).apply();
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

    public static void setLoginInfo(Context context, String name, String pwd) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putString(KEY_LOGIN_NAME, name)
                .putString(KEY_LOGIN_PWD, pwd).apply();
    }

    public static String getLoginName(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(KEY_LOGIN_NAME, null);
    }

    public static String getLoginPassword(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(KEY_LOGIN_PWD, null);
    }


    public static void setNetworkId(Context context, int userId, int networkId) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_NETWORK_ID + userId, networkId).apply();
    }

    /**
     * get network id cache
     */
    public static int getNetworkId(Context context, int userId) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(KEY_NETWORK_ID + userId, 0);
    }


    /**
     * get network id cache
     */
    public static String getClientId(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(DEFAULT_NAME, Context.MODE_PRIVATE);
        String id = sharedPreferences.getString(KEY_CLIENT_ID, null);
        if (id == null) {
            id = UUID.randomUUID().toString();
            sharedPreferences.edit().putString(KEY_CLIENT_ID, id).apply();
        }
        return id;
    }
}
