/********************************************************************************************************
 * @file HeartbeatPubSetMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * heartbeat publication set
 * Created by kee on 2019/6/19.
 */
public class HeartbeatPubSetMessage implements ModelMessage {

    // 2 bytes
    public int destination;

    public byte countLog;

    public byte periodLog;

    public byte ttl;

    // 2 bytes
    public int features;

    // 2 bytes
    public int netKeyIndex;


    public static HeartbeatPubSetMessage createInstance(int destination, byte countLog, byte periodLog, byte ttl,
                                                        int features, int netKeyIndex) {
        HeartbeatPubSetMessage instance = new HeartbeatPubSetMessage();
        instance.destination = destination;
        instance.countLog = countLog;
        instance.periodLog = periodLog;
        instance.ttl = ttl;
        instance.features = features;
        instance.netKeyIndex = netKeyIndex;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) destination)
                .put(countLog)
                .put(periodLog)
                .put(ttl)
                .putShort((short) features)
                .putShort((short) netKeyIndex);

        return byteBuffer.array();
    }
}
