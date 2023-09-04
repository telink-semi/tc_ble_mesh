/********************************************************************************************************
 * @file MeshLoopTask.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.util;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;

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
