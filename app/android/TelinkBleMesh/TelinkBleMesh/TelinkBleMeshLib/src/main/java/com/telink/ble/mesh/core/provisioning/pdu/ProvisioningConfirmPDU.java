package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/19.
 */

public class ProvisioningConfirmPDU implements ProvisioningStatePDU {


    public byte[] confirm;

    public ProvisioningConfirmPDU(byte[] confirm) {
        this.confirm = confirm;
    }

    @Override
    public byte[] toBytes() {
        return confirm;
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_CONFIRMATION;
    }
}
