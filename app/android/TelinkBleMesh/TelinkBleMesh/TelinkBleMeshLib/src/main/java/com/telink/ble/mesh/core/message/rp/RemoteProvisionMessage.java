package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.config.ConfigMessage;

import androidx.annotation.IntRange;

public class RemoteProvisionMessage extends ConfigMessage {
    public RemoteProvisionMessage(int destinationAddress) {
        super(destinationAddress);
    }
}
