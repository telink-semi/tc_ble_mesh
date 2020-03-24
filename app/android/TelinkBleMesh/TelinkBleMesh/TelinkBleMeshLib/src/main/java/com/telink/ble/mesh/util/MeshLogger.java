package com.telink.ble.mesh.util;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class MeshLogger {

    public static final int LEVEL_VERBOSE = 0;

    public static final int LEVEL_DEBUG = 1;

    public static final int LEVEL_INFO = 2;

    public static final int LEVEL_WARN = 3;

    public static final int LEVEL_ERROR = 4;


    public static List<LogInfo> logInfoList = new ArrayList<>();


    public static final String GLOBAL_TAG = "SIG-Mesh";

    public static final String LOG_NONE_TAG = "NONE-TAG";

    public static final String TAG_SEPARATOR = " -- ";

    public static final int DEFAULT_LEVEL = LEVEL_DEBUG;

    // Logger.getGlobal();
//    public static Logger logger = Logger.getLogger(GLOBAL_TAG);

    private static boolean print = true;

    private static boolean record = false;


    public static void enableRecord(boolean enable) {
        record = enable;
    }

    public static void enablePrint(boolean enable) {
        print = enable;
    }

    public static void log(String logMessage) {
        log(logMessage, LOG_NONE_TAG);
    }

    public static void log(String logMessage, String tag) {
        log(logMessage, tag, DEFAULT_LEVEL);
    }

    public static void log(String logMessage, int level) {
        log(logMessage, LOG_NONE_TAG, level);
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
                    Log.v(GLOBAL_TAG, tag + TAG_SEPARATOR + logMessage);
                    break;
                case LEVEL_INFO:
                    Log.i(GLOBAL_TAG, tag + TAG_SEPARATOR + logMessage);
                    break;

                case LEVEL_WARN:
                    Log.w(GLOBAL_TAG, tag + TAG_SEPARATOR + logMessage);
                    break;

                case LEVEL_ERROR:
                    Log.e(GLOBAL_TAG, tag + TAG_SEPARATOR + logMessage);
                    break;

                case LEVEL_DEBUG:
                default:
                    Log.d(GLOBAL_TAG, tag + TAG_SEPARATOR + logMessage);
            }
        }

        if (record) {
            logInfoList.add(new LogInfo(tag, logMessage, level));
        }
    }
}
