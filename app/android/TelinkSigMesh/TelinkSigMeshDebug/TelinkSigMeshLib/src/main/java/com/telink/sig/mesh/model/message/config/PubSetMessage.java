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
import com.telink.sig.mesh.model.message.TransitionTime;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * set publish info
 * mesh_v1.0#4.3.2.16 Config Model Publication Set
 * Created by kee on 2019/3/4.
 */
public class PubSetMessage implements ModelMessage {


    public static final int CREDENTIAL_FLAG_DEFAULT = 0b1;

    public static final int RFU_DEFAULT = 0x0C;


    // 采用固件端的默认配置
    public static final int TTL_DEFAULT = 0xFF;

    /**
     * default retransmit: 0x15
     * 0b 00010 101
     * count: 0x05, step: 0x02
     */
    public static final int RETRANSMIT_COUNT_DEFAULT = 0x05;


    public static final int RETRANSMIT_INTERVAL_STEP_DEFAULT = 0x02;


    /**
     * element address
     * 16 bits
     */
    public int elementAddress;

    /**
     * 16 bits
     */
    public int publishAddress;

    /**
     * 12 bits
     */
    public int appKeyIndex;

    /**
     * 1 bit
     */
    public int credentialFlag = CREDENTIAL_FLAG_DEFAULT;

    /**
     * 3 bits
     * reserved for future use
     */
    public int rfu = RFU_DEFAULT;

    /**
     * 8 bits
     */
    public int ttl = TTL_DEFAULT;

    /**
     * 8 bits
     */
    public byte period;

    /**
     * 3 bits
     */
    public int retransmitCount = RETRANSMIT_COUNT_DEFAULT;

    /**
     * 5 bits
     */
    public int retransmitIntervalSteps = RETRANSMIT_INTERVAL_STEP_DEFAULT;

    /**
     * 16 or 32 bits
     */
    public int modelId;

    /**
     * if sig model
     * #modelId
     */
    public boolean sig;

    public static PubSetMessage createDefault(int elementAddress, int publishAddress, int appKeyIndex, long periodMillisecond, int modelId, boolean sig) {
        PubSetMessage instance = new PubSetMessage();
        instance.elementAddress = elementAddress;
        instance.publishAddress = publishAddress;
        instance.appKeyIndex = appKeyIndex;
        instance.period = TransitionTime.fromTime(periodMillisecond).getValue();
        instance.modelId = modelId;
        instance.sig = sig;
        return instance;
    }


    public static PubSetMessage createInstance(int elementAddress,
                                        int publishAddress,
                                        int appKeyIndex,
                                        int credentialFlag, int rfu, int ttl,
                                        byte period,
                                        int retransmitCount, int retransmitIntervalSteps,
                                        int modelId, boolean sig) {
        PubSetMessage instance = new PubSetMessage();
        instance.elementAddress = elementAddress;
        instance.publishAddress = publishAddress;
        instance.appKeyIndex = appKeyIndex;
        instance.credentialFlag = credentialFlag;
        instance.rfu = rfu;
        instance.ttl = ttl;
        instance.period = period;
        instance.retransmitCount = retransmitCount;
        instance.retransmitIntervalSteps = retransmitIntervalSteps;
        instance.modelId = modelId;
        instance.sig = sig;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        final int len = sig ? 11 : 13;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        bf
                .putShort((short) elementAddress)
                .putShort((short) publishAddress)
                .putShort((short) ((appKeyIndex & 0xFFF) | ((credentialFlag & 0b01) << 12) | ((rfu & 0b111) << 13)))
                .put((byte) ttl)
                .put(period)
                .put((byte) ((retransmitCount & 0b111) | (retransmitIntervalSteps << 3)));
        if (sig) {
            bf.putShort((short) modelId);
        } else {
            bf.putInt(modelId);
        }
        return bf.array();
    }
}
