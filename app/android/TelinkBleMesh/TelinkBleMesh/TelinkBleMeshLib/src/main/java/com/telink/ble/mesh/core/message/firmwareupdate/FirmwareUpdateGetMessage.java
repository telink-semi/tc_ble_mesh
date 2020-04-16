package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;


public class FirmwareUpdateGetMessage extends UpdatingMessage {


    public static FirmwareUpdateGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateGetMessage message = new FirmwareUpdateGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public FirmwareUpdateGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }



}
