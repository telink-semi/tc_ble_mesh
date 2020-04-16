package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


public class BlobTransferGetMessage extends UpdatingMessage {


    public static BlobTransferGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        BlobTransferGetMessage message = new BlobTransferGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public BlobTransferGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }



}
