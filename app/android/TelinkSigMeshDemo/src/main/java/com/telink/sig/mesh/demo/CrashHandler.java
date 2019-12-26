/********************************************************************************************************
 * @file CrashHandler.java
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
package com.telink.sig.mesh.demo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.Thread.UncaughtExceptionHandler;
import java.text.SimpleDateFormat;
import java.util.Date;


public class CrashHandler implements UncaughtExceptionHandler {

    private UncaughtExceptionHandler handler;

    private static CrashHandler crashHandler;

    public static CrashHandler init(Context context) {
        if (crashHandler == null) {
            crashHandler = new CrashHandler(context);
        }
        return crashHandler;
    }

    private CrashHandler(Context context) {

        handler = Thread.getDefaultUncaughtExceptionHandler();

        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    @SuppressLint("SimpleDateFormat")
    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        SimpleDateFormat simpledateformat = new SimpleDateFormat(
                "dd-MM-yyyy hh:mm:ss");

        // 日期、app版本信息
        StringBuilder buff = new StringBuilder();
        buff.append("Date: ").append(simpledateformat.format(new Date()))
                .append("\n");
        buff.append("========MODEL:" + Build.MODEL + " \n");

        // 崩溃的堆栈信息
        buff.append("Stacktrace:\n\n");
        StringWriter stringwriter = new StringWriter();
        PrintWriter printwriter = new PrintWriter(stringwriter);
        throwable.printStackTrace(printwriter);
        buff.append(stringwriter.toString());
        buff.append("===========\n");
        printwriter.close();

        write2ErrorLog(buff.toString());

        if (handler != null) {
            // 交给还给系统处理
            handler.uncaughtException(thread, throwable);
        }
        //Process.killProcess(Process.myPid());
        //System.exit(0);
    }


    private void write2ErrorLog(String content) {
        String name = "crash_" + System.currentTimeMillis() + ".txt";
        File file = new File(FileSystem.getSettingPath(), name);
        FileOutputStream fos = null;
        try {
            if (file.exists()) {
                // 清空之前的记录
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
