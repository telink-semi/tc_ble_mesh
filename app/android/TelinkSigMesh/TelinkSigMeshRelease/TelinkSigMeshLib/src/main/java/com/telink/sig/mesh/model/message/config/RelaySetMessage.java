/********************************************************************************************************
 * @file Group.java
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
package com.telink.sig.mesh.model.message.config;

import com.telink.sig.mesh.model.message.ModelMessage;

/**
 * relay set
 * Mesh_v1.0 4.3.2.13 Config Relay Set
 * Created by kee on 2019/3/4.
 */
public class RelaySetMessage implements ModelMessage {

    /**
     * 1 bytes
     */
    public byte relay;

    /**
     * Number of retransmissions on advertising bearer for each Network PDU relayed by the node
     * 3 bits
     */
    public int relayRetransmitCount;

    /**
     * Number of 10-millisecond steps between retransmissions
     * 5bits
     */
    public int relayRetransmitIntervalSteps;


    public static RelaySetMessage createInstance(byte relay, int relayRetransmitCount, int relayRetransmitIntervalSteps) {
        RelaySetMessage instance = new RelaySetMessage();
        instance.relay = relay;
        instance.relayRetransmitCount = relayRetransmitCount;
        instance.relayRetransmitIntervalSteps = relayRetransmitIntervalSteps;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        return
                new byte[]{relay,
                        (byte) ((relayRetransmitCount & 0b111) | (relayRetransmitIntervalSteps << 3)),
                };
    }
}
