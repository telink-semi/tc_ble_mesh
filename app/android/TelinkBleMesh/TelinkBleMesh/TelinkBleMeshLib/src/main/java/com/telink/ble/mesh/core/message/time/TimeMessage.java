package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

public abstract class TimeMessage extends MeshMessage {

    public TimeMessage(int destinationAddress, int appKeyIndex) {
        this.destinationAddress = destinationAddress;
        this.appKeyIndex = appKeyIndex;
        this.accessType = AccessType.APPLICATION;
    }


}
