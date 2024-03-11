/********************************************************************************************************
 * @file UpdatingMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * The UpdatingMessage class is an abstract class that extends the MeshMessage class.
 * It represents a message used for updating purposes in a mesh network.
 * <p>
 * This class provides a constructor that initializes the destination address and application key index
 * of the message. It also sets the access type to APPLICATION.
 */
public abstract class UpdatingMessage extends MeshMessage {
    /**
     * Constructs a new UpdatingMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public UpdatingMessage(int destinationAddress, int appKeyIndex) {
        this.destinationAddress = destinationAddress;
        this.appKeyIndex = appKeyIndex;
        this.accessType = AccessType.APPLICATION;
    }

}
