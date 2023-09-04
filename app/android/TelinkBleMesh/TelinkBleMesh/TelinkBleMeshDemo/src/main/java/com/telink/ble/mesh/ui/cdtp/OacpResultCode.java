package com.telink.ble.mesh.ui.cdtp;

/**
 * OACP opcode
 */
public enum OacpResultCode {

    /**
     * Reserved For Future Use
     */
    RFU((byte) 0x00),


    /**
     * Response for successful operation.
     */
    SUCCESS((byte) 0x01),

    /**
     * Response if unsupported Op Code is received.
     */
    OPCODE_NOT_SUPPORTED((byte) 0x02),

    /**
     * Response if Parameter received does not meet the requirements of the service.
     */
    INVALID_PARAMETER((byte) 0x03),

    /**
     * Response if the number of octets requested via the value of the
     * Length parameter or Size parameter (as applicable) exceeds the
     * available memory or processing capabilities of the Server.
     */
    INSUFFICIENT_RESOURCES((byte) 0x04),

    /**
     * Response if the requested OACP procedure cannot be performed
     * because the Current Object is an Invalid Object.
     */
    INVALID_OBJECT((byte) 0x05),


    /**
     * Response if the requested procedure could not be performed
     * because an Object Transfer Channel was not available for use.
     */
    CHANNEL_UNAVAILABLE((byte) 0x06),


    /**
     * Response if the object type specified in the OACP procedure
     * Type parameter is not supported by the Server.
     */
    UNSUPPORTED_TYPE((byte) 0x07),


    /**
     * Response if the requested procedure is not permitted according
     * to the properties of the Current Object (refer to Section 3.2.8).
     */
    PROCEDURE_NOT_PERMITTED((byte) 0x08),

    /**
     * Response if the Current Object is temporarily locked by the
     * Server.
     */
    OBJECT_LOCKED((byte) 0x09),


    /**
     * Response if the requested procedure failed for any reason other
     * than those enumerated in this table.
     */
    OPERATION_FAILED((byte) 0x0A),
    ;

    public byte code;

    OacpResultCode(byte code) {
        this.code = code;
    }

}
