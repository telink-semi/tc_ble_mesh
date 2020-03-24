package com.telink.ble.mesh.util;


import java.io.Serializable;

/**
 * Created by kee on 2019/1/11.
 */

public class LogInfo implements Serializable {
    public String tag;
    public long millis;
    public int level;
    public String logMessage;
    public long threadId;
    public String threadName;

    public LogInfo(String tag, String logMessage, int level) {
        this.tag = tag;
        this.level = level;
        this.logMessage = logMessage;
        this.millis = System.currentTimeMillis();
        this.threadId = Thread.currentThread().getId();
        this.threadName = Thread.currentThread().getName();
    }


}
