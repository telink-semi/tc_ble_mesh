package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;


import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MeshSetAddressMessage extends GenericMessage {

    private byte[] mac;
    private int newMeshAddress;

    public static MeshSetAddressMessage getSimple(int destinationAddress, int appKeyIndex, byte[] mac, int newMeshAddress) {
        MeshSetAddressMessage message = new MeshSetAddressMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.mac = mac;
        message.newMeshAddress = newMeshAddress;
        return message;
    }

    public MeshSetAddressMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_ADDR_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_ADDR_SET_STS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).put(mac).putShort((short) newMeshAddress).array();
    }

}
