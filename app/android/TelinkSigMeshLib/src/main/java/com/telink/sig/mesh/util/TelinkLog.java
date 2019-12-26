/********************************************************************************************************
 * @file     TelinkLog.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.util;

import android.util.Log;

public class TelinkLog {

    public final static String TAG = "TelinkBluetoothSDK";

    public static boolean ENABLE = true;

    public static boolean isLoggable(int level) {
        if (ENABLE)
            return Log.isLoggable(TAG, level);
        return false;
    }

    public static String getStackTraceString(Throwable th) {
        if (ENABLE)
            return Log.getStackTraceString(th);
        return th.getMessage();
    }

    public static int println(int level, String msg) {
        if (ENABLE)
            return Log.println(level, TAG, msg);
        return 0;
    }

    public static int v(String msg) {
        if (ENABLE)
            return Log.v(TAG, msg);
        return 0;
    }

    public static int v(String msg, Throwable th) {
        if (ENABLE)
            return Log.v(TAG, msg, th);
        return 0;
    }

    public static int d(String msg) {
        if (ENABLE)
            return Log.d(TAG, msg);
        return 0;
    }

    public static int d(String msg, Throwable th) {
        if (ENABLE)
            return Log.d(TAG, msg, th);
        return 0;
    }

    public static int i(String msg) {
        if (ENABLE)
            return Log.i(TAG, msg);
        return 0;
    }

    public static int i(String msg, Throwable th) {
        if (ENABLE)
            return Log.i(TAG, msg, th);
        return 0;
    }

    public static int w(String msg) {
        if (ENABLE)
            return Log.w(TAG, msg);
        return 0;
    }

    public static int w(String msg, Throwable th) {
        if (ENABLE)
            return Log.w(TAG, msg, th);
        return 0;
    }

    public static int w(Throwable th) {
        if (ENABLE)
            return Log.w(TAG, th);
        return 0;
    }

    public static int e(String msg) {
        if (ENABLE)
            return Log.e(TAG, msg);
        return 0;
    }

    public static int e(String msg, Throwable th) {
        if (ENABLE)
            return Log.e(TAG, msg, th);
        return 0;
    }
}
