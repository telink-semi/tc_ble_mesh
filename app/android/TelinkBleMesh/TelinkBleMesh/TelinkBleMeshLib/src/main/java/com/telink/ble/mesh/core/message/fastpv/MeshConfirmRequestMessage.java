package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;


public class MeshConfirmRequestMessage extends GenericMessage {

    public static MeshConfirmRequestMessage getSimple(int destinationAddress, int appKeyIndex) {
        MeshConfirmRequestMessage message = new MeshConfirmRequestMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(0);
        message.setRetryCnt(1);
        return message;
    }

    public MeshConfirmRequestMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_CONFIRM.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_PROV_CONFIRM_STS.value;
    }

    @Override
    public byte[] getParams() {
        return null;
    }

}
