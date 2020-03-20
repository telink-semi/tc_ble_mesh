package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

public class ObjectInfoGetMessage extends UpdatingMessage {

    public static ObjectInfoGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        ObjectInfoGetMessage message = new ObjectInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public ObjectInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.OBJ_INFO_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OBJ_INFO_STATUS.value;
    }
}
