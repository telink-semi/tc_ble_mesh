package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/12.
 */

public class AppKeyAddMessage extends ConfigMessage {


    private int netKeyIndex;

    private int appKeyIndex;

    private byte[] appKey;

    public AppKeyAddMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }


    public void setAppKey(byte[] appKey) {
        this.appKey = appKey;
    }

    @Override
    public int getOpcode() {
        return Opcode.APPKEY_ADD.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.APPKEY_STATUS.value;
    }

    @Override
    public byte[] getParams() {

        // netKey index lower 12 bits
        // appKey index higher 12 bits

        int netAppKeyIndex = (netKeyIndex & 0x0FFF) | ((appKeyIndex & 0x0FFF) << 12);
//        int netAppKeyIndex = ((netKeyIndex & 0x0FFF) << 12) | ((appKeyIndex & 0x0FFF));
        byte[] indexesBuf = MeshUtils.integer2Bytes(netAppKeyIndex, 3, ByteOrder.LITTLE_ENDIAN);

        ByteBuffer paramsBuffer = ByteBuffer.allocate(3 + 16).order(ByteOrder.LITTLE_ENDIAN)
                .put(indexesBuf)
                .put(appKey);
        return paramsBuffer.array();
    }


}
