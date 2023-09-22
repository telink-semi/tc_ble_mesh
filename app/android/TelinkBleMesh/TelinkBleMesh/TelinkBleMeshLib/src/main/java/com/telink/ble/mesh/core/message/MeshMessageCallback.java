/********************************************************************************************************
 * @file MeshMessageCallback.java
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
package com.telink.ble.mesh.core.message;

import java.util.List;


/** 
 * A callback interface for handling the completion and response of a mesh message. 
 * todo
 */ 
public interface MeshMessageCallback {
    /** 
     * Called when a mesh message has been completed. 
     * 
     * @param meshMessage The completed mesh message. 
     * @param success     Indicates whether the message was successfully sent. 
     * @param responseCount The number of responses received for the message. 
     */ 
    void onMessageComplete(MeshMessage meshMessage, boolean success, int responseCount);
    /** 
     * Called when a response is received for a mesh message. 
     * 
     * @param meshMessage         The original mesh message. 
     * @param notificationMessages The list of notification messages received as response. 
     */ 
    void onMessageResponse(MeshMessage meshMessage, List<NotificationMessage> notificationMessages);
}
