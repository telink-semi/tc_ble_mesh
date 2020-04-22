package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;


import java.nio.ByteOrder;

public class ResetNetworkMessage extends GenericMessage {

    /**
     * milliseconds
     * 2 bytes
     */
    private int delay;

    public static ResetNetworkMessage getSimple(int destinationAddress, int appKeyIndex, int delay) {
        ResetNetworkMessage message = new ResetNetworkMessage(destinationAddress, appKeyIndex);
        message.delay = delay;
        return message;
    }

    public ResetNetworkMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_RESET_NETWORK.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(delay, 2, ByteOrder.LITTLE_ENDIAN);
    }

    public void setDelay(int delay) {
        this.delay = delay;
    }
}
