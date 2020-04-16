package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class BlobTransferCancelMessage extends UpdatingMessage {

    /**
     * BLOB identifier
     * 64 bits
     */
    public long blobId;

    public static BlobTransferCancelMessage getSimple(int destinationAddress, int appKeyIndex,
                                                      long blobId) {
        BlobTransferCancelMessage message = new BlobTransferCancelMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blobId = blobId;
        return message;
    }

    public BlobTransferCancelMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_CANCEL.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(16).order(ByteOrder.LITTLE_ENDIAN)
                .putLong(blobId)
                .array();
    }

}
