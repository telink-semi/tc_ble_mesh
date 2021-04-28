package com.telink.ble.mesh.model;

import android.content.Context;

import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.FileSystem;

import java.io.Serializable;
import java.util.List;

/**
 * firmware update cache,
 * saved when distribution started by remote device,
 * retrieved when mesh reconnect success
 */
public class FUCache implements Serializable {

    /**
     * distributor address, should be valid unicast address
     */
    public int distAddress;

    /**
     * firmware update policy
     */
    public UpdatePolicy updatePolicy;

    /**
     * updating devices
     */
    public List<MeshUpdatingDevice> updatingDeviceList;

    /**
     * target firmware id
     */
    public byte[] firmwareId;

    @Override
    public String toString() {
        return "FUCache{" +
                "distAddress=" + distAddress +
                ", updatePolicy=" + updatePolicy +
                ", updatingDeviceList=" + updatingDeviceList.size() +
                '}';
    }
}
