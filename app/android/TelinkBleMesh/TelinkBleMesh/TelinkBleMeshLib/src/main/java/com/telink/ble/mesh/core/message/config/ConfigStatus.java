package com.telink.ble.mesh.core.message.config;

public enum ConfigStatus {

    SUCCESS(0x00, "Success"),

    /**
     * error code
     */
    INVALID_ADDRESS(0x01, "Invalid Address"),

    INVALID_MODEL(0x02, "Invalid Model"),

    INVALID_APPKEY_INDEX(0x03, "Invalid AppKey Index"),

    INVALID_NETKEY_INDEX(0x04, "Invalid NetKey Index"),

    INSUFFICIENT_RESOURCES(0x05, "Insufficient Resources"),

    KEY_INDEX_ALREADY_STORED(0x06, "Key Index Already Stored"),

    INVALID_PUBLISH_PARAMETERS(0x07, "Invalid Publish Parameters"),

    NOT_A_SUBSCRIBE_MODEL(0x08, "Not a Subscribe Model"),

    STORAGE_FAILURE(0x09, "Storage Failure"),

    FEATURE_NOT_SUPPORTED(0x0A, "Feature Not Supported"),

    CANNOT_UPDATE(0x0B, "Cannot Update"),

    CANNOT_REMOVE(0x0C, "Cannot Remove"),

    CANNOT_BIND(0x0D, "Cannot Bind"),

    TEMPORARILY_UNABLE_TO_CHANGE_STATE(0x0E, "Temporarily Unable to Change State"),

    CANNOT_SET(0x0F, "Cannot Set"),

    UNSPECIFIED_ERROR(0x10, "Unspecified Error"),

    INVALID_BINDING(0x11, "Invalid Binding"),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    ConfigStatus(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static ConfigStatus valueOf(int code) {
        for (ConfigStatus status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
