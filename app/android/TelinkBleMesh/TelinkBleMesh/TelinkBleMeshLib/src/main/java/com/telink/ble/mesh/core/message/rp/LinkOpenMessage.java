package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.updating.UpdatingMessage;

public class LinkOpenMessage extends UpdatingMessage {

    /**
     * 16 bytes
     */
    private byte[] uuid;

    public static LinkOpenMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax, byte[] uuid) {
        LinkOpenMessage message = new LinkOpenMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        message.uuid = uuid;
        return message;
    }

    public LinkOpenMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_LINK_OPEN.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_LINK_STS.value;
    }

    @Override
    public byte[] getParams() {
        return uuid;
    }

}
