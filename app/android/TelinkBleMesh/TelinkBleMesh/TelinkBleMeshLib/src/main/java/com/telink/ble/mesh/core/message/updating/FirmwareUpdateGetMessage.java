package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FirmwareUpdateGetMessage extends UpdatingMessage {


    /**
     * 2 bytes
     */
    private int companyId;

    /**
     * 4 bytes
     */
    private int firmwareId;

    public static FirmwareUpdateGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                     int companyId,
                                                     int firmwareId) {
        FirmwareUpdateGetMessage message = new FirmwareUpdateGetMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(rspMax);
        message.companyId = companyId;
        message.firmwareId = firmwareId;
        return message;
    }

    public FirmwareUpdateGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FW_UPDATE_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FW_UPDATE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(6).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) companyId)
                .putInt(firmwareId).array();
    }

    public void setCompanyId(int companyId) {
        this.companyId = companyId;
    }

    public void setFirmwareId(int firmwareId) {
        this.firmwareId = firmwareId;
    }
}
