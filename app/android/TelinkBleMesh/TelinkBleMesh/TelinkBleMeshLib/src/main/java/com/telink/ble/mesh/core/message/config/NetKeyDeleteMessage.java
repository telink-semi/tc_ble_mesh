/********************************************************************************************************
 * @file NetKeyDeleteMessage.java
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
 * The Config NetKey Delete is an acknowledged message used to delete a NetKey on a NetKey List from a node.
 * <p>
 * The response to a Config NetKey Delete message is a Config NetKey Status message.
 */
public class NetKeyDeleteMessage extends ConfigMessage {


    public int netKeyIndex;


    public NetKeyDeleteMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public NetKeyDeleteMessage(int destinationAddress, int netKeyIndex) {
        super(destinationAddress);
        this.netKeyIndex = netKeyIndex;
    }

    @Override
    public int getOpcode() {
        return Opcode.NETKEY_DEL.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.NETKEY_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer paramsBuffer = ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) this.netKeyIndex);
        return paramsBuffer.array();
    }


}
