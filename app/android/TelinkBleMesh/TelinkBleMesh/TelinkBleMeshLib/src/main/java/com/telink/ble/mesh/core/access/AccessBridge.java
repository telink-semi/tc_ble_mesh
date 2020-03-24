package com.telink.ble.mesh.core.access;

import com.telink.ble.mesh.core.message.MeshMessage;

/**
 * Created by kee on 2019/9/11.
 */

public interface AccessBridge {

    /**
     * BINDING flow
     */
    int MODE_BINDING = 1;

    /**
     * firmware updating (mesh ota)
     */
    int MODE_FIRMWARE_UPDATING = 2;

    // remote provision
    int MODE_REMOTE_PROVISIONING = 3;

    int MODE_FAST_PROVISION = 4;

    /**
     * prepared to send mesh message
     *
     * @return if message sent
     */
    boolean onAccessMessagePrepared(MeshMessage meshMessage, int mode);

    /**
     * @param state binding state
     * @param desc  desc
     */
    void onAccessStateChanged(int state, String desc, int mode, Object obj);
}
