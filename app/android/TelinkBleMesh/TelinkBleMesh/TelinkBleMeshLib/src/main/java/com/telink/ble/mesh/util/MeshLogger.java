/********************************************************************************************************
 * @file MeshLogger.java
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

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * used to output and cache log information
 */
public class MeshLogger {

    /**
     * @see Log#v(String, String)
     */
    public static final int LEVEL_VERBOSE = 0;

    /**
     * @see Log#d(String, String)
     */
    public static final int LEVEL_DEBUG = 1;

    /**
     * @see Log#i(String, String)
     */
    public static final int LEVEL_INFO = 2;

    /**
     * @see Log#w(String, String)
     */
    public static final int LEVEL_WARN = 3;

    /**
     * @see Log#e(String, String)
     */
    public static final int LEVEL_ERROR = 4;

    /**
     * used to cache log info list
     */
    public static List<LogInfo> logInfoList = new ArrayList<>();

    /**
     * default log tag
     */
    public static final String DEFAULT_TAG = "SIG-Mesh";

    /**
     * default level: debug
     */
    public static final int DEFAULT_LEVEL = LEVEL_DEBUG;

//    public static Logger logger = Logger.getLogger(GLOBAL_TAG);

    /**
     * Used to set whether to output
     */
    private static boolean print = true;

    /**
     * Used to set whether to cache
     */
    private static boolean record = true;


    /**
     * @param enable new value
     */
    public static void enableRecord(boolean enable) {
        record = enable;
    }

    /**
     * @param enable new value
     */
    public static void enablePrint(boolean enable) {
        print = enable;
    }

    public static void log(String logMessage) {
        log(logMessage, DEFAULT_TAG);
    }

    public static void log(String logMessage, String tag) {
        log(logMessage, tag, DEFAULT_LEVEL);
    }

    public static void log(String logMessage, int level) {
        log(logMessage, DEFAULT_TAG, level);
    }


    public static void v(String logMessage) {
        log(logMessage, LEVEL_VERBOSE);
    }

    public static void d(String logMessage) {
        log(logMessage, LEVEL_DEBUG);
    }

    public static void i(String logMessage) {
        log(logMessage, LEVEL_INFO);
    }

    public static void w(String logMessage) {
        log(logMessage, LEVEL_WARN);
    }

    public static void e(String logMessage) {
        log(logMessage, LEVEL_ERROR);
    }


    public static void log(String logMessage, String tag, int level) {
        if (print) {
            switch (level) {
                case LEVEL_VERBOSE:
                    Log.v(tag, logMessage);
                    break;
                case LEVEL_INFO:
                    Log.i(tag, logMessage);
                    break;

                case LEVEL_WARN:
                    Log.w(tag, logMessage);
                    break;

                case LEVEL_ERROR:
                    Log.e(tag, logMessage);
                    break;

                case LEVEL_DEBUG:
                default:
                    Log.d(tag, logMessage);
            }
        }

        if (record) {
            logInfoList.add(new LogInfo(tag, logMessage, level));
        }
    }
}
