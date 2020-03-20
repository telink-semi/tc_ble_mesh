package com.telink.ble.mesh.core.networking.transport.lower;

/**
 * Created by kee on 2019/8/22.
 */

public abstract class TransportControlMessagePDU extends LowerTransportPDU {
    public static final int CONTROL_MESSAGE_OPCODE_SEG_ACK = 0x00;


    /*
    other values defined in
    spec#3.6.5.11 Summary of opcodes
     */

    public static final int CONTROL_MESSAGE_OPCODE_HEARTBEAT = 0x0A;

}
