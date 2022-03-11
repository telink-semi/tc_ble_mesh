/********************************************************************************************************
 * @file AccessBridge.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
