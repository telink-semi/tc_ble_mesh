package com.telink.ble.mesh.core.message;

import java.util.List;

/**
 * todo
 */
public interface MeshMessageCallback {

    void onMessageComplete(MeshMessage meshMessage, boolean success, int responseCount);

    void onMessageResponse(MeshMessage meshMessage, List<NotificationMessage> notificationMessages);
}
