package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.MeshUtils;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/23.
 */
public class HeartbeatStatus {

    /**
     * Reserved for Future Use
     */
    private int rfu;

    /**
     * 7 bits
     * Initial TTL used when sending the message
     */
    private int initTTL;

    /**
     * Bit field of currently active features of the node
     */
    private int features;

    public void parse(byte[] transportPdu) {
        this.rfu = (transportPdu[0] & 0xFF) >> 7;
        this.initTTL = transportPdu[0] & 0x7F;
        this.features = MeshUtils.bytes2Integer(new byte[]{transportPdu[1], transportPdu[2]},
                ByteOrder.BIG_ENDIAN);
    }


    public int getRfu() {
        return rfu;
    }

    public int getInitTTL() {
        return initTTL;
    }

    public int getFeatures() {
        return features;
    }
}
