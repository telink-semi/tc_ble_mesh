package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;


import java.nio.ByteOrder;

public class MeshProvisionCompleteMessage extends GenericMessage {

    /**
     * milliseconds
     * 2 bytes
     */
    private int delay;

    public static MeshProvisionCompleteMessage getSimple(int destinationAddress, int appKeyIndex, int delay) {
        MeshProvisionCompleteMessage message = new MeshProvisionCompleteMessage(destinationAddress, appKeyIndex);
        message.delay = delay;
        return message;
    }

    public MeshProvisionCompleteMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_COMPLETE.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(delay, 2, ByteOrder.LITTLE_ENDIAN);
    }

}
