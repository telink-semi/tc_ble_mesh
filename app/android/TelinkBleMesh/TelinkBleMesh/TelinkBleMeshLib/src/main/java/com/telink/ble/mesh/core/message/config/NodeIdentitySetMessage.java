package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/12.
 */

public class NodeIdentitySetMessage extends ConfigMessage {

    private int netKeyIndex;

    private int identity;

    public NodeIdentitySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.NODE_ID_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.NODE_ID_STATUS.value;
    }

    @Override
    public byte[] getParams() {

        // netKey index lower 12 bits
        int netAppKeyIndex = (netKeyIndex & 0x0FFF);
        ByteBuffer paramsBuffer = ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) (netKeyIndex & 0x0FFF))
                .put((byte) identity);
        return paramsBuffer.array();
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    public void setIdentity(int identity) {
        this.identity = identity;
    }
}
