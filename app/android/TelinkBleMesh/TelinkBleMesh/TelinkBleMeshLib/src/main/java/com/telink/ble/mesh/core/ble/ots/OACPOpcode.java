package com.telink.ble.mesh.core.ble.ots;

/**
 * OACP opcode
 */
public enum OACPOpcode {
    /**
     * Create
     * params: Size (UINT32), Type (gatt_uuid)
     */
    CREATE(0x01),

    /**
     * Delete
     * params: none
     */
    DELETE(0x02),


    /**
     * Calculate Checksum
     * params: Offset (UINT32), Length (UINT32)
     */
    CALCULATE_CHECKSUM(0x03),


    /**
     * Parameter may be defined by a higher-level spec;none otherwise.
     */
    Execute(0x04),


    /**
     * Offset (UINT32), Length (UINT32)
     */
    READ(0x05),

    /**
     * Write
     * Offset (UINT32), Length (UINT32), Mode (8bit)
     */
    WRITE(0x06),

    /**
     * Abort
     * no params
     */
    ABORT(0x07),

    /**
     * response code
     * params: OACP Response Value
     */
    RESPONSE(0x60);

    int code;

    OACPOpcode(int code) {
        this.code = code;
    }

}
