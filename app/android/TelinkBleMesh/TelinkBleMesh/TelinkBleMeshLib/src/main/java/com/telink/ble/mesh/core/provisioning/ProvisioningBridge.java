package com.telink.ble.mesh.core.provisioning;

/**
 * Created by kee on 2019/9/12.
 */

public interface ProvisioningBridge {

    /**
     * @param state provisioning state
     * @param desc  desc
     */
    void onProvisionStateChanged(int state, String desc);

    /**
     * gatt command prepared to sent
     *
     * @param type command type
     * @param data data
     */
    void onCommandPrepared(byte type, byte[] data);
}
