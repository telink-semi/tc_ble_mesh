package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;


public abstract class UpdatingMessage extends MeshMessage {

    public UpdatingMessage(int destinationAddress, int appKeyIndex) {
        this.destinationAddress = destinationAddress;
        this.appKeyIndex = appKeyIndex;
        this.accessType = AccessType.APPLICATION;
    }

}
