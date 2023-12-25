/********************************************************************************************************
 * @file AppCrashHandler.java
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

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.Thread.UncaughtExceptionHandler;
import java.text.SimpleDateFormat;
import java.util.Date;

public class AppCrashHandler implements UncaughtExceptionHandler {

    private final static String DISK_CACHE_PATH = "/TelinkBleMeshCrash/";

    private UncaughtExceptionHandler handler;

    private static AppCrashHandler crashHandler;

    public static AppCrashHandler init(Context context) {
        if (crashHandler == null) {
            crashHandler = new AppCrashHandler(context);
        }
        return crashHandler;
    }

    private AppCrashHandler(Context context) {

        handler = Thread.getDefaultUncaughtExceptionHandler();

        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    @SuppressLint("SimpleDateFormat")
    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        SimpleDateFormat simpledateformat = new SimpleDateFormat(
                "dd-MM-yyyy hh:mm:ss");

        StringBuilder buff = new StringBuilder();
        buff.append("Date: ").append(simpledateformat.format(new Date()))
                .append("\n");
        buff.append("========MODEL:" + Build.MODEL + " \n");

        // stack info
        buff.append("Stacktrace:\n\n");
        StringWriter stringwriter = new StringWriter();
        PrintWriter printwriter = new PrintWriter(stringwriter);
        throwable.printStackTrace(printwriter);
        buff.append(stringwriter.toString());
        buff.append("===========\n");
        printwriter.close();

        write2ErrorLog(buff.toString());

        if (handler != null) {
            handler.uncaughtException(thread, throwable);
        }
    }


    /**
     * 创建总文件夹
     */
    public String getFilePath() {
        String cachePath;
        if (Environment.getExternalStorageState().equals(
                Environment.MEDIA_MOUNTED)) {
            cachePath = Environment.getExternalStorageDirectory()
                    + DISK_CACHE_PATH;
        } else {
            cachePath = TelinkMeshApplication.getInstance().getCacheDir() + DISK_CACHE_PATH;
        }
        File file = new File(cachePath);
        if (!file.exists()) {
            file.mkdirs();
        }
        return cachePath;
    }

    private void write2ErrorLog(String content) {
        SimpleDateFormat simpledateformat = new SimpleDateFormat(
                "yyyyMMddhhmmss");
        String fileName = "/Crash_" + simpledateformat.format(new Date()) + ".txt";
        File file = new File(getFilePath() + fileName);
        FileOutputStream fos = null;
        try {
            if (file.exists()) {
                file.delete();
            } else {
                file.getParentFile().mkdirs();
            }
            file.createNewFile();
            fos = new FileOutputStream(file);
            fos.write(content.getBytes());
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fos != null) {
                    fos.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


}
