package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;


import java.nio.ByteOrder;

public class MeshGetAddressMessage extends GenericMessage {

    private int pid;

    public static MeshGetAddressMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax, int pid) {
        MeshGetAddressMessage message = new MeshGetAddressMessage(destinationAddress, appKeyIndex);
        message.setRetryCnt(0);
        message.setResponseMax(rspMax);
        message.pid = pid;
        return message;
    }

    public MeshGetAddressMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_ADDR_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_ADDR_GET_STS.value;
    }

    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(this.pid, 2, ByteOrder.LITTLE_ENDIAN);
    }


    public void setPid(int pid) {
        this.pid = pid;
    }
}
