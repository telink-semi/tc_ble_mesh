package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class BlobTransferStartMessage extends UpdatingMessage {

    /**
     * BLOB transfer mode
     * higher 2 bits in first byte
     */
    public TransferMode transferMode = TransferMode.PUSH;

    /**
     * BLOB identifier
     * 64 bits
     */
    public long blobId;

    /**
     * BLOB size in octets
     * 32 bits
     */
    public int blobSize;

    /**
     * Indicates the block size
     * 8 bits
     */
    public int blockSizeLog;

    /**
     * MTU size supported by the client
     */
    public int clientMTUSize;


    public static BlobTransferStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                     long blobId,
                                                     int blobSize,
                                                     byte blockSizeLog,
                                                     int clientMTUSize) {
        BlobTransferStartMessage message = new BlobTransferStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blobId = blobId;
        message.blobSize = blobSize;
        message.blockSizeLog = blockSizeLog;
        message.clientMTUSize = clientMTUSize;
        return message;
    }

    public BlobTransferStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        final byte modeValue = (byte) (transferMode.mode << 6);

        return ByteBuffer.allocate(16).order(ByteOrder.LITTLE_ENDIAN)
                .put(modeValue)
                .putLong(blobId)
                .putInt(blobSize)
                .put((byte) blockSizeLog)
                .putShort((short) clientMTUSize).array();
    }

}
