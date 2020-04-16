package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

public enum TransferPhase {

    INACTIVE(0x00,
            "The BLOB Transfer Server is awaiting configuration and cannot receive a BLOB."),

    WAITING_FOR_TRANSFER_START(0x01,
            "The BLOB Transfer Server is ready to receive the BLOB identified by the Expected BLOB ID."),

    WAITING_FOR_NEXT_BLOCK(0x02,
            "The BLOB Transfer Server is waiting for the next block of data."),

    WAITING_FOR_NEXT_CHUNK(0x03,
            "The BLOB Transfer Server is waiting for the next chunk of data."),

    COMPLETE(0x04, "The BLOB was transferred successfully."),

    SUSPENDED(0x05, "The Initialize and Receive BLOB procedure is paused."),

    UNKNOWN_ERROR(0x0A, "unknown error");

    public final int code;
    public final String desc;

    TransferPhase(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static TransferPhase valueOf(int code) {
        for (TransferPhase status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
