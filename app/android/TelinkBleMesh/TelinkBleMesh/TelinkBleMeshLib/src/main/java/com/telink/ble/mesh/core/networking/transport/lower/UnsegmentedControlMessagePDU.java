/********************************************************************************************************
 * @file UnsegmentedControlMessagePDU.java
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
package com.telink.ble.mesh.core.networking.transport.lower;

import java.nio.ByteBuffer;

/**
 * Created by kee on 2019/8/16.
 */

public class UnsegmentedControlMessagePDU extends LowerTransportPDU {

    /**
     * 1 bit
     */
    final int seg = 0;

    /**
     * 0x00 = Segment Acknowledgment
     * 0x01 to 0x7F = Opcode of the Transport Control message
     * 7 bits
     */
    private int opcode;

    /**
     * 0 ~ 88 bits
     */
    byte[] params;


    @Override
    public byte[] toByteArray() {
        byte header = (byte) ((seg << 7) | (opcode));
        if (params == null) {
            return new byte[]{header};
        }
        ByteBuffer byteBuffer = ByteBuffer.allocate(1 + params.length);
        byteBuffer.put(header).put(params);
        return byteBuffer.array();
    }


    @Override
    public int getType() {
        return TYPE_UNSEGMENTED_CONTROL_MESSAGE;
    }

    @Override
    public boolean segmented() {
        return false;
    }
}
