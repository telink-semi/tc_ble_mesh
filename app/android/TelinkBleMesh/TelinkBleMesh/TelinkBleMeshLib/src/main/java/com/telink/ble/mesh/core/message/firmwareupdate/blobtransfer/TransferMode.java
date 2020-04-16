package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

/**
 * The Transfer Mode state is a 2-bit value that indicates the mode of the BLOB transfer
 */
public enum TransferMode {

    NONE(0x00, "No Active Transfer"),

    PUSH(0x01, "Push BLOB Transfer Mode"),

    PULL(0x02, "Pull BLOB Transfer Mode"),

    Prohibited(0x03, "Prohibited");

    public final int mode;
    public final String desc;

    TransferMode(int mode, String desc) {
        this.mode = mode;
        this.desc = desc;
    }

    public static TransferMode valueOf(int mode) {
        for (TransferMode status : values()) {
            if (status.mode == mode) return status;
        }
        return Prohibited;
    }
}
