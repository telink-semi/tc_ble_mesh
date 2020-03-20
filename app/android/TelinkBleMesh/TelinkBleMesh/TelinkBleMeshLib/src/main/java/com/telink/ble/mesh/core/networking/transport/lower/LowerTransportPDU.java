package com.telink.ble.mesh.core.networking.transport.lower;

import com.telink.ble.mesh.core.networking.NetworkingPDU;

/**
 * big endian
 * transport: access message, control message
 * Created by kee on 2019/7/22.
 */
public abstract class LowerTransportPDU implements NetworkingPDU {

    public static int TYPE_UNSEGMENTED_ACCESS_MESSAGE = 0xb00;

    public static int TYPE_SEGMENTED_ACCESS_MESSAGE = 0xb01;

    public static int TYPE_UNSEGMENTED_CONTROL_MESSAGE = 0xb10;

    public static int TYPE_SEGMENTED_CONTROL_MESSAGE = 0xb11;


    public static int SEG_TYPE_UNSEGMENTED = 0;

    public static int SEG_TYPE_SEGMENTED = 1;

    protected int seg;

    /**
     * get pdu typeValue
     *
     * @return PDU typeValue
     */
    public abstract int getType();

    /**
     * segmented state, determined by seg bit significant
     *
     * @return if is a segment pdu
     */
    public abstract boolean segmented();


}
