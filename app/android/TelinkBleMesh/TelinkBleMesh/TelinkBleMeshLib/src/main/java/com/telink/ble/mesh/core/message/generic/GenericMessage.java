package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * generic model message
 * Created by kee on 2019/8/14.
 */

public abstract class GenericMessage extends MeshMessage {

    public GenericMessage(int destinationAddress, int appKeyIndex) {
        this.destinationAddress = destinationAddress;
        this.appKeyIndex = appKeyIndex;
        this.accessType = AccessType.APPLICATION;
    }
}
