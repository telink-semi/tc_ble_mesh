package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;


public class FirmwareUpdateApplyMessage extends UpdatingMessage {


    public static FirmwareUpdateApplyMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateApplyMessage message = new FirmwareUpdateApplyMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public FirmwareUpdateApplyMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_APPLY.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }

    

}
