package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FirmwareUpdateStartMessage extends UpdatingMessage {

    /**
     * Do not apply new firmware when Object transfer is completed.
     */
    public static final byte POLICY_NONE = 0x00;

    /**
     * Apply new firmware when Object transfer is completed.
     */
    public static final byte POLICY_AUTO_UPDATE = 0x01;

    private byte updatePolicy;

    /**
     * 2 bytes
     */
    private int companyId;

    /**
     * 4 bytes
     */
    private int firmwareId;

    public static FirmwareUpdateStartMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                       byte updatePolicy,
                                                       int companyId,
                                                       int firmwareId) {
        FirmwareUpdateStartMessage message = new FirmwareUpdateStartMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(rspMax);
        message.companyId = companyId;
        message.firmwareId = firmwareId;
        message.updatePolicy = updatePolicy;
        return message;
    }

    public FirmwareUpdateStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FW_UPDATE_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FW_UPDATE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                .put(updatePolicy)
                .putShort((short) companyId)
                .putInt(firmwareId).array();
    }

    public void setCompanyId(int companyId) {
        this.companyId = companyId;
    }

    public void setFirmwareId(int firmwareId) {
        this.firmwareId = firmwareId;
    }

    public void setUpdatePolicy(byte updatePolicy) {
        this.updatePolicy = updatePolicy;
    }
}
