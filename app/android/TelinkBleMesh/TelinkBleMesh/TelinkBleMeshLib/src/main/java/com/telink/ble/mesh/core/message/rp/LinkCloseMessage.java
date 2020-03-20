package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.updating.UpdatingMessage;

public class LinkCloseMessage extends UpdatingMessage {


    public static final byte REASON_SUCCESS = 0x00;

    public static final byte REASON_PROHIBITED = 0x01;

    public static final byte REASON_FAIL = 0x02;

    /**
     * 1 byte
     */
    private byte reason;

    public static LinkCloseMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax, byte reason) {
        LinkCloseMessage message = new LinkCloseMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        message.reason = reason;
        return message;
    }

    public LinkCloseMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_LINK_CLOSE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_LINK_STS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{reason};
    }

}
