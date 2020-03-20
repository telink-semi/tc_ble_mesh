package com.telink.ble.mesh.util;

import java.io.Serializable;
import java.util.Date;

/**
 * Created by kee on 2019/1/11.
 */

public class LogInfo implements Serializable {
    public String tag;
    public Date datetime;
    public LogLevel level;
    public String log;

    public LogInfo(String log) {
        this("NO-TAG", log);
    }

    public LogInfo(String tag, String log) {
        this(tag, LogLevel.DEBUG, log);
    }

    public LogInfo(String tag, LogLevel level, String log) {
        this.tag = tag;
        this.level = level;
        this.log = log;
        this.datetime = new Date();
    }

    public enum LogLevel implements Serializable {
        VERBOSE, DEBUG, INFO, WARN, ERROR
    }

}
