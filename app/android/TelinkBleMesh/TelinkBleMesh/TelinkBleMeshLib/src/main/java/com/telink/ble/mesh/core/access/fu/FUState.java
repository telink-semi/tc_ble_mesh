package com.telink.ble.mesh.core.access.fu;


/**
 * firmware-update state
 */
public enum FUState {
    IDLE(0x0100,
            "initiating distributor ..."
    ),

    INITIATING(0,
            "initiating distributor ..."
    ),

    INITIATE_SUCCESS(1,
            "initiate distributor success"
    ),

    INITIATE_FAIL(2,
            "initiate distributor fail"
    ),

    DISTRIBUTING_BY_PHONE(3,
            "distributing by phone..."),

    DISTRIBUTING_BY_DEVICE(4,
            "distributing by device..."),

    /**
     * applying devices update
     */
    UPDATE_APPLYING(7,
            "firmware update applying"),

    /**
     * confirm distribute
     */
    DISTRIBUTE_CONFIRMING(8,
            "getting distribute status util complete"),

    /**
     * recheck firmware id
     */
    UPDATE_RECHECKING(9,
            "firmware update rechecking..."),


    UPDATE_COMPLETE(10,
            "update complete"),

    UPDATE_FAIL(11,
            "update fail"),

    TRANSFER_START(12,
            "blob transfer started"),
    ;

    public final int value;

    public final String desc;

    FUState(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
