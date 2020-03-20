package com.telink.ble.mesh.core.proxy;

/**
 * Created by kee on 2019/8/26.
 */

public abstract class ProxyConfigurationMessage {


    /**
     * Set Filter Type
     * Sent by a Proxy Client to set the proxy filter typeValue.
     */
    public static final byte OPCODE_SET_FILTER_TYPE = 0x00;

    /**
     * Add Addresses To Filter
     * Sent by a Proxy Client to add addresses to the proxy filter list.
     */
    public static final byte OPCODE_ADD_ADDRESS = 0x01;

    /**
     * Remove Addresses From Filter
     * Sent by a Proxy Client to remove addresses from the proxy filter list.
     */
    public static final byte OPCODE_REMOVE_ADDRESS = 0x02;

    /**
     * Filter Status
     * Acknowledgment by a Proxy Server to a Proxy Client to report the status of the proxy filter list.
     */
    public static final byte OPCODE_FILTER_STATUS = 0x03;


    public abstract byte getOpcode();

    public abstract byte[] toByteArray();

}
