package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/8/12.
 */

public class CompositionDataGetMessage extends ConfigMessage {


    public CompositionDataGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.COMPOSITION_DATA_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.COMPOSITION_DATA_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{(byte) 0xFF};
    }


}
