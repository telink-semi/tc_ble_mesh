/********************************************************************************************************
 * @file MeshProvisionCompleteMessage.java
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
package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteOrder;

public class MeshProvisionCompleteMessage extends GenericMessage {

    /**
     * milliseconds
     * 2 bytes
     */
    private int delay;

    public static MeshProvisionCompleteMessage getSimple(int destinationAddress, int appKeyIndex, int delay) {
        MeshProvisionCompleteMessage message = new MeshProvisionCompleteMessage(destinationAddress, appKeyIndex);
        message.delay = delay;
        return message;
    }

    public MeshProvisionCompleteMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_COMPLETE.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(delay, 2, ByteOrder.LITTLE_ENDIAN);
    }

    public void setDelay(int delay) {
        this.delay = delay;
    }
}
