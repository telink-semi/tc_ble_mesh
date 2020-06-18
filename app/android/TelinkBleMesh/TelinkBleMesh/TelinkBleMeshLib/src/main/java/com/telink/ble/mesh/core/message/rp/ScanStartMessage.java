package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ScanStartMessage extends RemoteProvisionMessage {

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

    public static ScanStartMessage getSimple(int destinationAddress, int rspMax, byte scannedItemsLimit, byte timeout) {
        ScanStartMessage message = new ScanStartMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.scannedItemsLimit = scannedItemsLimit;
        message.timeout = timeout;
        return message;
    }

    public ScanStartMessage(int destinationAddress) {
        super(destinationAddress);
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

    public void setScannedItemsLimit(byte scannedItemsLimit) {
        this.scannedItemsLimit = scannedItemsLimit;
    }

    public void setTimeout(byte timeout) {
        this.timeout = timeout;
    }

    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }
}
