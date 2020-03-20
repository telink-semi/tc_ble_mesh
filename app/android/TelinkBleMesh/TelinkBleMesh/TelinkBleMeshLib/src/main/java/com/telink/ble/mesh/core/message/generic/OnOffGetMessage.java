package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/8/20.
 */

public class OnOffGetMessage extends GenericMessage {


    public static OnOffGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        OnOffGetMessage message = new OnOffGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public OnOffGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.G_ONOFF_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.G_ONOFF_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return null;
    }

    @Override
    public boolean isContainsTid() {
        return false;
    }

}
