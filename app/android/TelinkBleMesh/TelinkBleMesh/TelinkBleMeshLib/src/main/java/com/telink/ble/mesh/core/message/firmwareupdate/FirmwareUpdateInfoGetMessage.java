package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;

public class FirmwareUpdateInfoGetMessage extends UpdatingMessage {

    /**
     * Index of the first requested entry from the Firmware Information List state
     * 1 byte
     */
    public byte firstIndex;

    /**
     * Maximum number of entries that the server includes in a Firmware Update Information Status message
     * 1 byte
     */
    public byte entriesLimit;


    public static FirmwareUpdateInfoGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateInfoGetMessage message = new FirmwareUpdateInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.firstIndex = 0;
        message.entriesLimit = 1;
        return message;
    }

    public FirmwareUpdateInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{firstIndex, entriesLimit};
    }

}
