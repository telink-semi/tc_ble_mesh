package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;


public class FirmwareUpdateCancelMessage extends UpdatingMessage {


    public static FirmwareUpdateCancelMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateCancelMessage message = new FirmwareUpdateCancelMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public FirmwareUpdateCancelMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_CANCEL.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }

    

}
