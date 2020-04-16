package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;
import com.telink.ble.mesh.util.Arrays;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class BlobChunkTransferMessage extends UpdatingMessage {


    /**
     * The chunk’s number in a set of chunks in a block
     * 2 bytes
     */
    public int chunkNumber;

    /**
     * Binary data from the current block
     */
    public byte[] chunkData;


    public static BlobChunkTransferMessage getSimple(int destinationAddress, int appKeyIndex,
                                                     int chunkNumber,
                                                     byte[] chunkData) {
        BlobChunkTransferMessage message = new BlobChunkTransferMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.chunkNumber = chunkNumber;
        message.chunkData = chunkData;
        return message;
    }

    public BlobChunkTransferMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_CHUNK_TRANSFER.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(2 + chunkData.length).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) chunkNumber)
                .put(chunkData);
        return bf.array();
    }

    @Override
    public String toString() {
        return "BlobChunkTransferMessage{" +
                "chunkNumber=" + chunkNumber +
                ", chunkData=" + Arrays.bytesToHexString(chunkData) +
                '}';
    }
}
