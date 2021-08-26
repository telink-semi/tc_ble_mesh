/********************************************************************************************************
 * @file ModelAppBindMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * The Config Relay Set is an acknowledged message used to set the Relay and Relay Retransmit states of a node.
 */
public class RelaySetMessage extends ConfigMessage {

    /**
     * 0x00 The node support Relay feature that is disabled
     * 0x01 The node supports Relay feature that is enabled
     * 0x02 Relay feature is not supported
     * 8 bits (1 byte)
     */
    public byte relay;

    /**
     * 3 bits
     */
    public byte retransmitCount;

    /**
     * Number of 10-millisecond steps between retransmissions
     * retransmission interval = (Relay Retransmit Interval Steps + 1) * 10
     * 5 bits
     */
    public byte retransmitIntervalSteps;

    public RelaySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static RelaySetMessage getSimple(int destinationAddress, byte relay, byte retransmitCount, byte retransmitIntervalSteps) {
        RelaySetMessage instance = new RelaySetMessage(destinationAddress);
        instance.relay = relay;
        instance.retransmitCount = retransmitCount;
        instance.retransmitIntervalSteps = retransmitIntervalSteps;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_RELAY_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_RELAY_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{
                relay,
                (byte) ((retransmitCount & 0b111) | (retransmitIntervalSteps << 3))
        };
    }


}
