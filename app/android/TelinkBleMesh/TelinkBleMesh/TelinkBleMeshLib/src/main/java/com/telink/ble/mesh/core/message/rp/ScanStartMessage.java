package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.updating.UpdatingMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ScanStartMessage extends UpdatingMessage {

    /**
     * 1 bytes
     */
    private byte scannedItemsLimit;

    /**
     * 1 bytes
     */
    private byte timeout;

    /**
     * Device UUID (Optional)
     */
    private byte[] uuid;

    public static ScanStartMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax, byte scannedItemsLimit, byte timeout) {
        ScanStartMessage message = new ScanStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        message.scannedItemsLimit = scannedItemsLimit;
        message.timeout = timeout;
        return message;
    }

    public ScanStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_SCAN_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_SCAN_STS.value;
    }

    @Override
    public byte[] getParams() {
        int len = uuid == null ? 2 : 18;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .put(scannedItemsLimit).put(timeout);
        if (uuid != null) {
            bf.put(uuid);
        }
        return bf.array();
    }

}
