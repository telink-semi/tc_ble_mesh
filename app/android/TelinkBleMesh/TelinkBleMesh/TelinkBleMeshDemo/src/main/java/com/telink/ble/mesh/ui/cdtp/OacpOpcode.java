package com.telink.ble.mesh.ui.cdtp;

/**
 * OACP opcode
 */
public enum OacpOpcode {
    /**
     * Create
     * params: Size (UINT32), Type (gatt_uuid)
     */
    CREATE((byte) 0x01),

    /**
     * Delete
     * params: none
     */
    DELETE((byte) 0x02),


    /**
     * Calculate Checksum
     * params: Offset (UINT32), Length (UINT32)
     */
    CALCULATE_CHECKSUM((byte) 0x03),


    /**
     * Parameter may be defined by a higher-level spec;none otherwise.
     */
    Execute((byte) 0x04),


    /**
     * Offset (UINT32), Length (UINT32)
     */
    READ((byte) 0x05),

    /**
     * Write
     * Offset (UINT32), Length (UINT32), Mode (8bit)
     */
    WRITE((byte) 0x06),

    /**
     * Abort
     * no params
     */
    ABORT((byte) 0x07),

    /**
     * response code
     * params: OACP Response Value
     */
    RESPONSE((byte) 0x60);

    public byte code;

    OacpOpcode(byte code) {
        this.code = code;
    }

}
