package com.telink.ble.mesh.core.access.fu;

import com.telink.ble.mesh.entity.MeshUpdatingDevice;

public interface FUCallback {

    /**
     * log info
     */
    void onLog(String tag, String log, int logLevel);

    /**
     * firmware update state
     */
    void onStateUpdated(FUState state, String extraInfo);

    /**
     * device update fail or success
     */
    void onDeviceStateUpdate(MeshUpdatingDevice device, String desc);

    /**
     * blob transfer progress update
     */
    void onTransferProgress(int progress, BlobTransferType transferType);
}
