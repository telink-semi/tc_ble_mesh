package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/18.
 */

public interface ProvisioningStatePDU extends PDU {
    /**
     * @return provision state
     * @see ProvisioningPDU#type
     */
    byte getState();
}
