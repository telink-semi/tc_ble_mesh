package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Node reset
 * Created by kee on 2019/8/12.
 */

public class NodeResetMessage extends ConfigMessage {

    public NodeResetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.NODE_RESET.value;
    }

    @Override
    public byte[] getParams() {
        return null;
    }


}
