package com.telink.ble.mesh.util;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;

import java.util.List;

public class MeshLoopTask {

    List<MeshMessage> meshMessages;

    long minSpaceForUnreliable = 250;

    long minWaitingForReliable = 50;
    private int msgIndex;

    List<NotificationMessage> notificationMessages;

    void onMeshNotification(NotificationMessage notificationMessage) {
        final int index = msgIndex;
        if (meshMessages.size() > index) {
            final MeshMessage meshMessage = meshMessages.get(index);
            if (meshMessage.getResponseOpcode() == notificationMessage.getOpcode()) {
                notificationMessages.add(notificationMessage);
            }
        }
    }

    void onReliableStop() {

    }

    private MeshMessage getCurrentMessage() {
        final int index = msgIndex;
        if (meshMessages.size() > index) {
            return meshMessages.get(index);
        }
        return null;
    }

    interface Callback {
        int onLoopComplete();
    }
}
