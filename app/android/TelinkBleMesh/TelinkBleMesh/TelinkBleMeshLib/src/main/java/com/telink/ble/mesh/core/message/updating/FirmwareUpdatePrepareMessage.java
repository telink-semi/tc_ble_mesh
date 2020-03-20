package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FirmwareUpdatePrepareMessage extends UpdatingMessage {

    /**
     * 2 bytes
     */
    private int companyId;

    /**
     * 4 bytes
     */
    private int firmwareId;
    /**
     * 8 bytes
     */
    private long objectId;

    /**
     * optional: 0 byte
     */
    private byte[] vendorValidationData;

    public static FirmwareUpdatePrepareMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                         int companyId,
                                                         int firmwareId,
                                                         long objectId,
                                                         byte[] vendorValidationData) {
        FirmwareUpdatePrepareMessage message = new FirmwareUpdatePrepareMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(rspMax);
        message.companyId = companyId;
        message.firmwareId = firmwareId;
        message.objectId = objectId;
        message.vendorValidationData = vendorValidationData;
        return message;
    }

    public FirmwareUpdatePrepareMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FW_UPDATE_PREPARE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FW_UPDATE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(2 + 4 + 8 + (vendorValidationData == null ? 0 : vendorValidationData.length)).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) companyId)
                .putInt(firmwareId)
                .putLong(objectId);
        if (vendorValidationData != null) {
            bf.put(vendorValidationData);
        }
        return bf.array();
    }

    public void setCompanyId(int companyId) {
        this.companyId = companyId;
    }

    public void setFirmwareId(int firmwareId) {
        this.firmwareId = firmwareId;
    }

    public void setObjectId(long objectId) {
        this.objectId = objectId;
    }

    public void setVendorValidationData(byte[] vendorValidationData) {
        this.vendorValidationData = vendorValidationData;
    }
}
