package com.telink.ble.mesh.core.access.fu;

import com.telink.ble.mesh.core.message.MeshMessage;

interface BlobTransferCallback {

    /*
     * state update
     *
     * @param state new state
     */
//    void onTransferStateUpdate(int state, String desc);

    /**
     * progress of chunk sending
     *
     * @param progress 0-100
     */
    void onTransferProgressUpdate(int progress, BlobTransferType transferType);

    /**
     * blob transfer complete
     *
     * @param success if at least one device blob transfer success
     */
    void onTransferComplete(boolean success, String desc);

    /**
     * @param address failed device address
     */
    void onTransferringDeviceFail(int address, String desc);

    /**
     * holder should send message
     *
     * @return if message sent
     */
    boolean onMeshMessagePrepared(MeshMessage meshMessage);

    void onTransferLog(String tag, String log, int logLevel);
}
