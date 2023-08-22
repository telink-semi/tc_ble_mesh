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
package com.telink.ble.mesh.core.message.sarconfig;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * A SAR_TRANSMITTER_GET message is an acknowledged message used to get the current SAR Transmitter state of a node.
 */
public class SarTransmitGetMessage extends ConfigMessage {

    public SarTransmitGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_SAR_TRANSMITTER_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_SAR_TRANSMITTER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return null;
    }


}
