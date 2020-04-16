package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FirmwareUpdateStartMessage extends UpdatingMessage {

    /**
     * default ttl
     */
    public static final byte DEFAULT_UPDATE_TTL = 10;

    /**
     * Time To Live value to use during firmware image transfer
     * 1 byte
     */
    public byte updateTtl = DEFAULT_UPDATE_TTL;

    /**
     * Used to compute the timeout of the firmware image transfer
     * Client Timeout = [12,000 * (Client Timeout Base + 1) + 100 * Transfer TTL] milliseconds
     * using blockSize
     * 2 bytes
     */
    public int updateTimeoutBase;

    /**
     * BLOB identifier for the firmware image
     * 8 bytes
     */
    public long updateBLOBID;

    /**
     * Index of the firmware image in the Firmware Information List state to be updated
     * 1 byte
     */
    public int updateFirmwareImageIndex;

    /**
     * Vendor-specific firmware metadata
     * If the value of the Incoming Firmware Metadata Length field is greater than 0,
     * the Incoming Firmware Metadata field shall be present.
     * 1-255 bytes
     */
    public byte[] metadata;

    public static FirmwareUpdateStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                       int updateTimeoutBase, long blobId, byte[] metadata) {
        FirmwareUpdateStartMessage message = new FirmwareUpdateStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.updateTimeoutBase = updateTimeoutBase;
        message.updateBLOBID = blobId;
        message.updateFirmwareImageIndex = 0;
        message.metadata = metadata;
        return message;
    }

    public FirmwareUpdateStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        final int len = 12 + metadata.length;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .put(updateTtl).putShort((short) updateTimeoutBase)
                .putLong(updateBLOBID).put((byte) updateFirmwareImageIndex).put(metadata);
        return bf.array();
    }

}
