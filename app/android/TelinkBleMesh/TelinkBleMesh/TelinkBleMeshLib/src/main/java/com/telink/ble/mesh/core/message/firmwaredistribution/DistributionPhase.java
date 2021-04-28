package com.telink.ble.mesh.core.message.firmwaredistribution;


/**
 * firmware-update state
 */
public enum DistributionPhase {
    IDLE(0x00,
            "No firmware distribution is in progress."
    ),

    TRANSFER_ACTIVE(0x01,
            "Firmware distribution is in progress."
    ),

    TRANSFER_SUCCESS(0x02,
            "The Transfer BLOB procedure has completed successfully."
    ),

    APPLYING_UPDATE(0x03,
            "The Apply Firmware on Updating Nodes procedure is being executed."
    ),

    COMPLETED(0x04,
            "The Distribute Firmware procedure has completed successfully."
    ),

    FAILED(0x05,
            "The Distribute Firmware procedure has failed."
    ),

    CANCELING_UPDATE(0x06,
            "The Cancel Firmware Update procedure is being executed."
    ),
    ;

    public final int value;

    public final String desc;

    DistributionPhase(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
