package com.telink.ble.mesh.core.message.config;

public enum NodeIdentity {
    STOPPED(0, "Node Identity for a subnet is stopped"),

    RUNNING(1, "Node Identity for a subnet is running"),

    UNSUPPORTED(2, "Node Identity is not supported"),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    NodeIdentity(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static NodeIdentity valueOf(int code) {
        for (NodeIdentity status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
