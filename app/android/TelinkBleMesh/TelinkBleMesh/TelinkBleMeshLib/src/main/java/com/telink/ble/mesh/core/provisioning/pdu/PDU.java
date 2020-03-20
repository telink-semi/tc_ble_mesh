package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/18.
 */

public interface PDU {

    /**
     * @return pdu as byte array
     */
    byte[] toBytes();
}
