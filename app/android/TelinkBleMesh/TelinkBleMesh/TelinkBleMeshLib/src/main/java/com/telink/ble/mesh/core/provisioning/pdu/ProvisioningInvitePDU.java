package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/19.
 */

public class ProvisioningInvitePDU implements ProvisioningStatePDU {


    // Attention Timer state
    public byte attentionDuration;

    public ProvisioningInvitePDU(byte attentionDuration) {
        this.attentionDuration = attentionDuration;
    }

    @Override
    public byte[] toBytes() {
        return new byte[]{attentionDuration};
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_INVITE;
    }
}
