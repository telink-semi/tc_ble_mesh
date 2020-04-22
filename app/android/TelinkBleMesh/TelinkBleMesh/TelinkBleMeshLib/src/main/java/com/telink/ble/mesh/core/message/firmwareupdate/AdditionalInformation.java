package com.telink.ble.mesh.core.message.firmwareupdate;

public enum AdditionalInformation {


    No_changes(0x00, "No changes to node composition data"),

    CHANGES_1(0x01, "Node composition data changed. The node does not support remote provisioning."),

    CHANGES_2(0x02, "Node composition data changed, and remote provisioning is supported. " +
            "The node supports remote provisioning and composition data page 0x80. Page 0x80 contains different composition data than page 0x0."),

    CHANGES_3(0x03, "Node unprovisioned. The node is unprovisioned after successful application of a verified firmware image."),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    AdditionalInformation(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static AdditionalInformation valueOf(int code) {
        for (AdditionalInformation status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
