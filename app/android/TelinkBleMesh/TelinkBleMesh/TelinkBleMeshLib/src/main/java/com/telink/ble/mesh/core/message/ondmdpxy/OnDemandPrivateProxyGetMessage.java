/********************************************************************************************************
 * @file CompositionDataGetMessage.java
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
 * represents a message that can be sent to request information about an on-demand private proxy.
 */
public class OnDemandPrivateProxyGetMessage extends ConfigMessage {

    public OnDemandPrivateProxyGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the OnDemandPrivateProxyGetMessage, which is Opcode.CFG_ON_DEMAND_PROXY_GET.
     *
     * @return The opcode value for the OnDemandPrivateProxyGetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_ON_DEMAND_PROXY_GET.value;
    }

    /**
     * Returns the response opcode value for the OnDemandPrivateProxyGetMessage, which is Opcode.CFG_ON_DEMAND_PROXY_GET.
     *
     * @return The response opcode value for the OnDemandPrivateProxyGetMessage.
     */

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_ON_DEMAND_PROXY_STATUS.value;
    }

    /**
     * Returns the parameters of the OnDemandPrivateProxyGetMessage.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return null;
    }


}
