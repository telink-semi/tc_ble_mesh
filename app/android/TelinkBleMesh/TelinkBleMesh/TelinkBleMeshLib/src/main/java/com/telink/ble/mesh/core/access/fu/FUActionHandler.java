package com.telink.ble.mesh.core.access.fu;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;

interface FUActionHandler {

    void onDeviceUpdate(MeshUpdatingDevice device, String desc);

    void onActionComplete(boolean success, FUAction action, String desc);

    boolean onMessagePrepared(MeshMessage meshMessage);

    void onTransferProgress(int progress, BlobTransferType transferType);

    void onActionLog(String tag, String log, int logLevel);
}
