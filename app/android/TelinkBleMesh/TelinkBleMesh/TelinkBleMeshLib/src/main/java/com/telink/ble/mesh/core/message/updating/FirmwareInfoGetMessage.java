package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

public class FirmwareInfoGetMessage extends UpdatingMessage {

    public static FirmwareInfoGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        FirmwareInfoGetMessage message = new FirmwareInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public FirmwareInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FW_INFO_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FW_INFO_STATUS.value;
    }
}
