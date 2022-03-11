/********************************************************************************************************
 * @file KeyRefreshPhaseGetMessage.java
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Key Refresh Phase Get is an acknowledged message used to get the current Key Refresh Phase state of the identified network key.
 */
public class KeyRefreshPhaseGetMessage extends ConfigMessage {

    /*
     * NetKey Index
     */
    public int netKeyIndex = 0;


    public KeyRefreshPhaseGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static KeyRefreshPhaseGetMessage getSimple(int destinationAddress, int netKeyIndex) {
        KeyRefreshPhaseGetMessage instance = new KeyRefreshPhaseGetMessage(destinationAddress);
        instance.netKeyIndex = netKeyIndex;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_KEY_REFRESH_PHASE_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_KEY_REFRESH_PHASE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN).putShort((short) netKeyIndex).array();
    }

}
