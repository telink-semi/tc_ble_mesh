package com.telink.ble.mesh.core.message.firmwaredistribution;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FDReceiversGetMessage extends UpdatingMessage {

    /**
     * first index
     * Index of the first requested entry from the Distribution Receivers List state
     * 2 bytes
     */
    public int firstIndex;

    /**
     * Entries Limit
     * Maximum number of entries that the server includes in a Firmware Distribution Receivers List message
     * 2 bytes
     */
    public int entriesLimit;

    public FDReceiversGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    public static FDReceiversGetMessage getSimple(int destinationAddress, int appKeyIndex, int firstIndex, int entriesLimit) {
        FDReceiversGetMessage message = new FDReceiversGetMessage(destinationAddress, appKeyIndex);
        message.firstIndex = firstIndex;
        message.entriesLimit = entriesLimit;
        message.setResponseMax(1);
        return message;
    }

    @Override
    public int getOpcode() {
        return Opcode.FD_RECEIVERS_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FD_RECEIVERS_LIST.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) firstIndex).putShort((short) entriesLimit).array();
    }
}
