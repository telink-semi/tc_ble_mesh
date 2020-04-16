package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


public class BlobBlockGetMessage extends UpdatingMessage {


    public static BlobBlockGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        BlobBlockGetMessage message = new BlobBlockGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public BlobBlockGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_BLOCK_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_BLOCK_STATUS.value;
    }



}
