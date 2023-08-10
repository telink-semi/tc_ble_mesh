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
 * The Config Composition Data Get is an acknowledged message used to read one page of the Composition Data
 * <p>
 * The response to a Config Composition Data Get message is a Config Composition Data Status message {@link CompositionDataStatusMessage}.
 */
public class SarTransmitSetMessage extends ConfigMessage {

    /**
     * New SAR Segment Interval Step state
     */
    public int segmentIntervalStep;

    /**
     * New SAR Unicast Retransmissions Count state
     */
    public int unicastRetransCnt;

    /**
     * New SAR Unicast Retransmissions Without Progress Count state
     */
    public int unicastRetransWithoutProgCnt;

    /**
     * New SAR Unicast Retransmissions Interval Step state
     */
    public int unicastRetransIntervalStep;

    /**
     * New SAR Unicast Retransmissions Interval Increment state
     */
    public int unicastRetransIntervalIncrement;

    /**
     * New SAR Multicast Retransmissions Count state
     */
    public int multicastRetransCnt;

    /**
     * New SAR Multicast Retransmissions Interval state
     */
    public int multicastRetransInterval;

    /**
     * Reserved for Future Use
     */
    public int rfu;


    public SarTransmitSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_SAR_TRANSMITTER_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_SAR_TRANSMITTER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                .put((byte) (segmentIntervalStep | (unicastRetransCnt << 4)))
                .put((byte) (unicastRetransWithoutProgCnt | (unicastRetransIntervalStep << 4)))
                .put((byte) (unicastRetransIntervalIncrement | (multicastRetransCnt << 4)))
                .put((byte) (multicastRetransInterval | (rfu << 4))).array();
    }


}
