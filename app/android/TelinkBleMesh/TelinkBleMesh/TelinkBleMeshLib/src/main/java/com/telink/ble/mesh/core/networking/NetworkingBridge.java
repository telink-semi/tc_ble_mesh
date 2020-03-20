package com.telink.ble.mesh.core.networking;

import com.telink.ble.mesh.core.proxy.ProxyPDU;

/**
 * Created by kee on 2019/8/15.
 */

public interface NetworkingBridge {

    /**
     * @param type proxy pdu typeValue {@link ProxyPDU#type}
     * @param data gatt data
     */
    void onCommandPrepared(byte type, byte[] data);

    /**
     * application layer should save updated network info
     */
    void onNetworkInfoUpdate(int sequenceNumber, int ivIndex);

    /**
     * mesh model message
     */
    void onMeshMessageReceived(int src, int dst, int opcode, byte[] params);

    /**
     * received proxy status message when set filter type, or add/remove address
     * @param address connected node unicast address
     */
    void onProxyInitComplete(boolean success, int address);


    /**
     * heartbeat message received
     *
     * @param data heartbeat data
     */
    void onHeartbeatMessageReceived(int src, int dst, byte[] data);

    /**
     * @param success  if response received
     * @param opcode   command opcode
     * @param rspMax   expect response max
     * @param rspCount received response count
     */
    void onReliableMessageComplete(boolean success, int opcode, int rspMax, int rspCount);
}
