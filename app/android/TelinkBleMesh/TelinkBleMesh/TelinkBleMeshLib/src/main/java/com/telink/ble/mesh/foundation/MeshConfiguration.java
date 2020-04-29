package com.telink.ble.mesh.foundation;

import android.util.SparseArray;

import java.util.Arrays;

/**
 * Mesh info use when provisioning/binding/auto connecting
 * NO variable can be NULL
 * Created by kee on 2019/9/6.
 */

public class MeshConfiguration {

    /**
     * network key index
     */
    public int netKeyIndex;

    /**
     * network key
     */
    public byte[] networkKey;

    /**
     * appKeyIndex and appKey map
     */
    public SparseArray<byte[]> appKeyMap;

    /**
     * iv index
     */
    public int ivIndex;

    /**
     * sequence number used in network pdu
     */
    public int sequenceNumber;

    /**
     * provisioner address
     */
    public int localAddress;

    /**
     * unicastAddress and deviceKey map, required for mesh configuration message
     */
    public SparseArray<byte[]> deviceKeyMap;

    public int getDefaultAppKeyIndex() {
        return appKeyMap.size() > 0 ? appKeyMap.keyAt(0) : 0;
    }

    public byte[] getDefaultAppKey() {
        return appKeyMap.size() > 0 ? appKeyMap.valueAt(0) : null;
    }
}
