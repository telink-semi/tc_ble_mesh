package com.telink.ble.mesh.core.message.updating;


import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.util.Arrays;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import androidx.annotation.NonNull;


public class ObjectChunkTransferMessage extends UpdatingMessage {


    private int chunkNumber;

    private byte[] firmwareImageData;


    public static ObjectChunkTransferMessage getSimple(int destinationAddress, int appKeyIndex,
                                                       int chunkNumber,
                                                       @NonNull
                                                               byte[] firmwareImageData) {
        ObjectChunkTransferMessage message = new ObjectChunkTransferMessage(destinationAddress, appKeyIndex);
        message.chunkNumber = chunkNumber;
        message.firmwareImageData = firmwareImageData;
        return message;
    }

    public ObjectChunkTransferMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.OBJ_CHUNK_TRANSFER.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(2 + firmwareImageData.length).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) chunkNumber)
                .put(firmwareImageData);
        return bf.array();
    }

    public void setChunkNumber(int chunkNumber) {
        this.chunkNumber = chunkNumber;
    }

    public void setFirmwareImageData(byte[] firmwareImageData) {
        this.firmwareImageData = firmwareImageData;
    }

    @Override
    public String toString() {
        return "ObjectChunkTransferMessage{" +
                "chunkNumber=" + chunkNumber +
                ", firmwareImageData=" + Arrays.bytesToHexString(firmwareImageData, "") +
                ", opcode=0x" + Integer.toHexString(getOpcode()) +
                ", sourceAddress=" + sourceAddress +
                ", destinationAddress=" + destinationAddress +
                '}';
    }
}
