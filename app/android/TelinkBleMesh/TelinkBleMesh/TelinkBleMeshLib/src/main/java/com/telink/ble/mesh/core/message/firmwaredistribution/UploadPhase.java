package com.telink.ble.mesh.core.message.firmwaredistribution;


/**
 * upload phase
 */
public enum UploadPhase {

    IDLE(0x00,
            "No firmware upload is in progress."
    ),

    TRANSFER_ACTIVE(0x01,
            "The Store Firmware or Store Firmware OOB  procedure is being executed."
    ),

    TRANSFER_ERROR(0x02,
            "The Store Firmware procedure or Store Firmware OOB procedure failed."
    ),

    TRANSFER_SUCCESS(0x03,
            "The Store Firmware procedure or the Store Firmware OOB procedure completed successfully."
    ),

    ;

    public final int value;

    public final String desc;

    UploadPhase(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
