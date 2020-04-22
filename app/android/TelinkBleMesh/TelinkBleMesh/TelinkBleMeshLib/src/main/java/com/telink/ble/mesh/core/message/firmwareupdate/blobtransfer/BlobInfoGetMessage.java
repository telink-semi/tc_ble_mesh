package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


public class BlobInfoGetMessage extends UpdatingMessage {


    public static BlobInfoGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        BlobInfoGetMessage message = new BlobInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    public BlobInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_INFORMATION_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_INFORMATION_STATUS.value;
    }



}
