package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/19.
 */

public class ProvisioningRandomPDU implements ProvisioningStatePDU {

    public byte[] random;

    public ProvisioningRandomPDU(byte[] confirm) {
        this.random = confirm;
    }

    @Override
    public byte[] toBytes() {
        return random;
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RANDOM;
    }
}
