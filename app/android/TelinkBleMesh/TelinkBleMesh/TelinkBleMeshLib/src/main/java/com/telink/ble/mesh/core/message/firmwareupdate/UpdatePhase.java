package com.telink.ble.mesh.core.message.firmwareupdate;

public enum UpdatePhase {

    IDLE(0x00, "Ready to start a Receive Firmware procedure"),

    TRANSFER_ERROR(0x01, "The Transfer BLOB procedure failed."),

    TRANSFER_ACTIVE(0x02, "The Receive Firmware procedure is being executed"),

    VERIFICATION_ACTIVE(0x03, "The Verify Firmware procedure is being executed"),

    VERIFICATION_SUCCESS(0x04, "The Verify Firmware procedure completed successfully."),

    VERIFICATION_FAILED(0x05, "The Verify Firmware procedure failed."),

    APPLY_ACTIVE(0x06, "The Apply New Firmware procedure is being executed."),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    UpdatePhase(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static UpdatePhase valueOf(int code) {
        for (UpdatePhase status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
