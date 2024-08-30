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

import com.telink.ble.mesh.core.networking.ExtendBearerMode;

import java.util.UUID;


/**
 * Created by kee on 2017/8/30.
 */

public class SharedPreferenceHelper {

    private static final String SHARE_PREFERENCE_NAME = "telink_shared";

    private static final String KEY_FIRST_LOAD = "com.telink.bluetooth.light.KEY_FIRST_LOAD";

    /**
     * selected {@link com.telink.ble.mesh.model.MeshInfo#id}
     */
    private static final String KEY_SELECTED_MESH_ID = "com.telink.bluetooth.light.KEY_SELECTED_MESH_ID";

    private static final String KEY_LOCATION_IGNORE = "com.telink.bluetooth.light.KEY_LOCATION_IGNORE";

    private static final String KEY_LOG_ENABLE = "com.telink.bluetooth.light.KEY_LOG_ENABLE";

    /**
     * scan device by private mode
     */
    private static final String KEY_PRIVATE_MODE = "com.telink.bluetooth.light.KEY_PRIVATE_MODE";

    private static final String KEY_LOCAL_UUID = "com.telink.bluetooth.light.KEY_PROVISIONER_UUID";

    private static final String KEY_PROVISION_MODE = "com.telink.bluetooth.light.KEY_PROVISION_MODE";

    private static final String KEY_NO_OOB = "com.telink.bluetooth.light.KEY_NO_OOB";

    private static final String KEY_EXTEND_BEARER_MODE = "com.telink.bluetooth.light.KEY_EXTEND_BEARER_MODE";

    private static final String KEY_DIST_ADR = "com.telink.bluetooth.light.KEY_DIST_ADR";

    private static final String KEY_APPLY_POLICY = "com.telink.bluetooth.light.KEY_APPLY_POLICY";

    private static final String KEY_LEVEL_SERVICE = "com.telink.bluetooth.light.KEY_LEVEL_SERVICE";

    private static final String KEY_SHARE_IMPORT_COMPLETE_ACTION = "com.telink.bluetooth.light.KEY_SHARE_IMPORT_COMPLETE_ACTION";

    private static final String KEY_BASE_URL = "com.telink.bluetooth.light.KEY_BASE_URL";

    // old : 47.115.40.63
    // new(20240830) 47.115.164.97
    public static final String DEFAULT_BASE_URL = "http://47.115.164.97:8080/";

    public static final boolean DEFAULT_LOG_ENABLE = true;


    public static final boolean DEFAULT_PRIVATE_MODE_ENABLE = false;

    public static final boolean DEFAULT_LEVEL_SERVICE_ENABLE = false;

    public static final boolean DEFAULT_AUTO_USE_NO_OOB_ENABLE = true;


    public static final int PROVISION_MODE_NORMAL_SELECTABLE = 0;

    public static final int PROVISION_MODE_NORMAL_AUTO = 1;

    public static final int PROVISION_MODE_REMOTE = 2;

    public static final int PROVISION_MODE_FAST = 3;

    public static final int DEFAULT_PROVISION_MODE = PROVISION_MODE_NORMAL_SELECTABLE;

    public static final int IMPORT_COMPLETE_ACTION_DEFAULT = 0;

    public static final int IMPORT_COMPLETE_ACTION_AUTO_SWITCH = 1;

    /**
     * sort type
     */
    private static final String KEY_NODE_SORT_TYPE = "com.telink.bluetooth.light.KEY_NODE_SORT_TYPE";

    public static boolean isLocationIgnore(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_LOCATION_IGNORE, false);
    }

    public static void setLocationIgnore(Context context, boolean ignore) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LOCATION_IGNORE, ignore).apply();
    }

    public static void resetAll(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit()
                .putBoolean(KEY_LOG_ENABLE, DEFAULT_LOG_ENABLE)
                .putBoolean(KEY_PRIVATE_MODE, DEFAULT_PRIVATE_MODE_ENABLE)
                .putInt(KEY_PROVISION_MODE, DEFAULT_PROVISION_MODE)
                .putBoolean(KEY_LEVEL_SERVICE, DEFAULT_LEVEL_SERVICE_ENABLE)
                .putString(KEY_EXTEND_BEARER_MODE, ExtendBearerMode.NONE.name())
                .putBoolean(KEY_NO_OOB, DEFAULT_AUTO_USE_NO_OOB_ENABLE)
                .putInt(KEY_SHARE_IMPORT_COMPLETE_ACTION, IMPORT_COMPLETE_ACTION_DEFAULT)
                .putString(KEY_BASE_URL, DEFAULT_BASE_URL)
                .apply();
    }

    public static boolean isLogEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_LOG_ENABLE, DEFAULT_LOG_ENABLE);
    }

    public static void setLogEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LOG_ENABLE, enable).apply();
    }

    public static boolean isPrivateMode(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_PRIVATE_MODE, DEFAULT_PRIVATE_MODE_ENABLE);
    }

    public static void setPrivateMode(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_PRIVATE_MODE, enable).apply();
    }

    public static String getLocalUUID(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        String uuid = sharedPreferences.getString(KEY_LOCAL_UUID, null);
        if (uuid == null) {
            uuid = UUID.randomUUID().toString().toUpperCase();
            sharedPreferences.edit().putString(KEY_LOCAL_UUID, uuid).apply();
        }
        return uuid;
    }


    public static int getProvisionMode(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(KEY_PROVISION_MODE, DEFAULT_PROVISION_MODE);
    }

    public static boolean isRemoteProvisionEnable(Context context) {
        return getProvisionMode(context) == PROVISION_MODE_REMOTE;
    }


    public static boolean isFastProvisionEnable(Context context) {
        return getProvisionMode(context) == PROVISION_MODE_FAST;
    }

    public static void setProvisionMode(Context context, int mode) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_PROVISION_MODE, mode).apply();
    }


    public static boolean isNoOOBEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_NO_OOB, DEFAULT_AUTO_USE_NO_OOB_ENABLE);
    }

    public static void setNoOOBEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_NO_OOB, enable).apply();
    }

    public static ExtendBearerMode getExtendBearerMode(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return ExtendBearerMode.valueOf(sharedPreferences.getString(KEY_EXTEND_BEARER_MODE, ExtendBearerMode.NONE.name()));
    }

    public static void setExtendBearerMode(Context context, ExtendBearerMode extendBearerMode) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putString(KEY_EXTEND_BEARER_MODE, extendBearerMode.name()).apply();
    }

    public static boolean isAutoPvEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(KEY_PROVISION_MODE, DEFAULT_PROVISION_MODE) == PROVISION_MODE_NORMAL_AUTO;
    }

    public static boolean isLevelServiceEnable(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(KEY_LEVEL_SERVICE, DEFAULT_LEVEL_SERVICE_ENABLE);
    }

    public static void setLevelServiceEnable(Context context, boolean enable) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putBoolean(KEY_LEVEL_SERVICE, enable).apply();
    }


    public static long getSelectedMeshId(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getLong(KEY_SELECTED_MESH_ID, -1);
    }

    /**
     * @param meshId {@link com.telink.ble.mesh.model.MeshInfo#id}
     */
    public static void setSelectedMeshId(Context context, long meshId) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putLong(KEY_SELECTED_MESH_ID, meshId).apply();
    }

    /**
     * action after share import
     *
     * @see #IMPORT_COMPLETE_ACTION_DEFAULT
     * @see #IMPORT_COMPLETE_ACTION_AUTO_SWITCH
     */
    public static int getShareImportCompleteAction(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(KEY_SHARE_IMPORT_COMPLETE_ACTION, IMPORT_COMPLETE_ACTION_DEFAULT);
    }

    public static void updateShareImportCompleteAction(Context context, int action) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_SHARE_IMPORT_COMPLETE_ACTION, action).apply();
    }


    public static int getNodeSortType(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(KEY_NODE_SORT_TYPE, 0);
    }

    public static void setNodeSortType(Context context, NodeSortType sortType) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putInt(KEY_NODE_SORT_TYPE, sortType.ordinal()).apply();
    }


    public static String getBaseUrl(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(KEY_BASE_URL, DEFAULT_BASE_URL);
    }

    public static void setBaseUrl(Context context, String baseUrl) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putString(KEY_BASE_URL, baseUrl).apply();
    }

    public static void resetBaseUrl(Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(SHARE_PREFERENCE_NAME, Context.MODE_PRIVATE);
        sharedPreferences.edit().putString(KEY_BASE_URL, DEFAULT_BASE_URL).apply();
    }


}
