package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/19.
 */

public class ProvisioningDataPDU implements ProvisioningStatePDU {

    // including mic
    public byte[] encryptedData;

    public ProvisioningDataPDU(byte[] encryptedData) {
        this.encryptedData = encryptedData;
    }

    @Override
    public byte[] toBytes() {
        return encryptedData;
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_DATA;
    }
}
