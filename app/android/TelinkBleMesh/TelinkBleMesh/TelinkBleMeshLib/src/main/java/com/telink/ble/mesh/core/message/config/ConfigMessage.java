package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

import androidx.annotation.IntRange;

/**
 * configuration message
 * Created by kee on 2019/8/14.
 */
public abstract class ConfigMessage extends MeshMessage {

    public ConfigMessage(@IntRange(from = 1, to = 0x7FFF) int destinationAddress) {
        this.destinationAddress = destinationAddress;
        // default rsp max
        this.responseMax = 1;
    }

    /**
     * for config message , AKF is 0
     *
     * @return application key flag
     */
    @Override
    public AccessType getAccessType() {
        return AccessType.DEVICE;
    }

}
