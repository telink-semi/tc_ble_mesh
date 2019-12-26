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
package com.telink.sig.mesh.model.message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/2/25.
 */

public class TimeMessage implements ModelMessage {

    /**
     * TAI seconds
     * 40 bits
     * The current TAI time in seconds
     */
    public long taiSeconds;

    /**
     * 8 bits
     * The sub-second time in units of 1/256th second
     */
    public byte subSecond;

    /**
     * 8 bits
     * The estimated uncertainty in 10 millisecond steps
     */
    public byte uncertainty;

    /**
     * 1 bit
     * 0 = No Time Authority, 1 = Time Authority
     */
    public byte timeAuthority;

    /**
     * TAI-UTC Delta
     * 15 bits
     * Current difference between TAI and UTC in seconds
     */
    public int delta;

    /**
     * Time Zone Offset
     * 8 bits
     * The local time zone offset in 15-minute increments
     */
    public int zoneOffset;

    public static TimeMessage createInstance(long timeSecond, int zoneOffset) {
        TimeMessage instance = new TimeMessage();
        instance.taiSeconds = timeSecond;
        instance.zoneOffset = zoneOffset;
//        instance.
        return instance;
    }

    @Override
    public byte[] toBytes() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN);

        byteBuffer.put((byte) (taiSeconds))
                .put((byte) (taiSeconds >> 8))
                .put((byte) (taiSeconds >> 16))
                .put((byte) (taiSeconds >> 24))
                .put((byte) (taiSeconds >> 32))
                .put(subSecond)
                .put(uncertainty)
                .putShort((short) ((delta << 1) | timeAuthority))
                .put((byte) (zoneOffset));
        return byteBuffer.array();
    }
}
