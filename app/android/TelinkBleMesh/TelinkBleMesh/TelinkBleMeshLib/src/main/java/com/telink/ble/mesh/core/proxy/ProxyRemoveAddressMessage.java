/********************************************************************************************************
 * @file ProxyRemoveAddressMessage.java
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
package com.telink.ble.mesh.core.proxy;


import androidx.annotation.NonNull;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/26.
 */

public class ProxyRemoveAddressMessage extends ProxyConfigurationMessage {

    private int[] addressArray;

    public ProxyRemoveAddressMessage(@NonNull int[] addressArray) {
        this.addressArray = addressArray;
    }

    @Override
    public byte getOpcode() {
        return OPCODE_REMOVE_ADDRESS;
    }

    @Override
    public byte[] toByteArray() {
        int len = 1 + addressArray.length * 2;
        ByteBuffer buffer = ByteBuffer.allocate(len).order(ByteOrder.BIG_ENDIAN).put(getOpcode());
        for (int address : addressArray) {
            buffer.putShort((short) address);
        }
        return buffer.array();
    }
}
