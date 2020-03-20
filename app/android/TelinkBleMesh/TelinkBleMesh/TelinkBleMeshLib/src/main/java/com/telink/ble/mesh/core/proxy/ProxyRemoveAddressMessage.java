package com.telink.ble.mesh.core.proxy;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import androidx.annotation.NonNull;

/**
 * Created by kee on 2019/8/26.
 */

public class ProxyRemoveAddressMessage extends ProxyConfigurationMessage {

    private int[] addressArray;

    public ProxyRemoveAddressMessage(@NonNull int[] addressArray) {
        this.addressArray = addressArray;
    }

    @Override
    public byte getOpcode() {
        return OPCODE_REMOVE_ADDRESS;
    }

    @Override
    public byte[] toByteArray() {
        int len = 1 + addressArray.length * 2;
        ByteBuffer buffer = ByteBuffer.allocate(len).order(ByteOrder.BIG_ENDIAN).put(getOpcode());
        for (int address : addressArray) {
            buffer.putShort((short) address);
        }
        return buffer.array();
    }
}
