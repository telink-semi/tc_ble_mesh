/********************************************************************************************************
 * @file OnDemandPrivateProxySetMessage.java
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
package com.telink.ble.mesh.core.message.ondmdpxy;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

/**
 * This class represents a message for setting the state of an on-demand private GATT proxy.
 * It extends the ConfigMessage class.
 */
public class OnDemandPrivateProxySetMessage extends ConfigMessage {

    /**
     * The new state of the on-demand private GATT proxy.
     */
    public byte privateProxy;

    /**
     * Constructs a new OnDemandPrivateProxySetMessage with the given destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public OnDemandPrivateProxySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_ON_DEMAND_PROXY_SET.value;
    }

    /**
     * Returns the response opcode of the message.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_ON_DEMAND_PROXY_STATUS.value;
    }

    /**
     * Returns the parameters of the message.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{privateProxy};
    }
}