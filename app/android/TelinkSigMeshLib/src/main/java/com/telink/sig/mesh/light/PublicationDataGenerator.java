/********************************************************************************************************
 * @file PublicationDataGenerator.java
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
package com.telink.sig.mesh.light;

import com.telink.sig.mesh.model.PublishModel;

/**
 * @see com.telink.sig.mesh.model.message.config.PubSetMessage
 * @deprecated
 * Created by kee on 2018/10/25.
 */

public class PublicationDataGenerator {


    public static final int CREDENTIAL_FLAG = 1;

    private static final int RFU = 12;

    // 采用固件端的默认配置
    public static final int TTL = 0xFF;


    // count = 2, invl_steps = 5
    public static final int TRANSMIT = 0x15;

    private static final int STEP_RESOLUTION_100_MILL = 0b00;
    private static final int STEP_RESOLUTION_1_SECOND = 0b01;
    private static final int STEP_RESOLUTION_10_SECOND = 0b10;
    private static final int STEP_RESOLUTION_10_MINUTE = 0b11;

    private static final int PERIOD_STEP_100_MILL = 100;
    private static final int PERIOD_STEP_1_SECOND = 1000;
    private static final int PERIOD_STEP_10_SECOND = 10 * 1000;
    private static final int PERIOD_STEP_10_MINUTE = 10 * 60 * 1000;

    private static final int MAX_STEP_VALUE = 0b111111;


    public static byte[] getSettingData(PublishModel publishModel, int appKeyIndex) {
        final int periodMillisecond = publishModel.period;
        final int ttl = publishModel.ttl;
        final int credential = publishModel.credential;
        final byte transmit = (byte) publishModel.transmit;
        int step;
        int cnt;
        if (periodMillisecond <= 0) {
            step = 0;
            cnt = 0;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_100_MILL) {
            step = STEP_RESOLUTION_100_MILL;
            cnt = periodMillisecond / PERIOD_STEP_100_MILL;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_1_SECOND) {
            step = STEP_RESOLUTION_1_SECOND;
            cnt = periodMillisecond / PERIOD_STEP_1_SECOND;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_SECOND) {
            step = STEP_RESOLUTION_10_SECOND;
            cnt = periodMillisecond / PERIOD_STEP_10_SECOND;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_MINUTE) {
            step = STEP_RESOLUTION_10_MINUTE;
            cnt = periodMillisecond / PERIOD_STEP_10_MINUTE;
        } else {
            return null;
        }

        byte[] keyIndex = new byte[2];
        keyIndex[0] = (byte) (appKeyIndex & 0xFF);
        keyIndex[1] = (byte) ((appKeyIndex >> 8) & 0xFF);

        return new byte[]{keyIndex[0], (byte) (((keyIndex[1] & 0b1111) | (credential & 0b1) << 4) | ((RFU & 0b111) << 5)), (byte) (ttl & 0xFF), (byte) ((cnt & MAX_STEP_VALUE) | ((step & 0b11) << 6)), transmit};
    }

    /**
     * @param periodMillisecond 0 disable period
     * @return param
     */
    public static byte[] getDefaultSettingData(int periodMillisecond, int appKeyIndex) {

//        millisecond /= 100;
        int step;
        int cnt;
        if (periodMillisecond <= 0) {
            step = 0;
            cnt = 0;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_100_MILL) {
            step = STEP_RESOLUTION_100_MILL;
            cnt = periodMillisecond / PERIOD_STEP_100_MILL;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_1_SECOND) {
            step = STEP_RESOLUTION_1_SECOND;
            cnt = periodMillisecond / PERIOD_STEP_1_SECOND;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_SECOND) {
            step = STEP_RESOLUTION_10_SECOND;
            cnt = periodMillisecond / PERIOD_STEP_10_SECOND;
        } else if (periodMillisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_MINUTE) {
            step = STEP_RESOLUTION_10_MINUTE;
            cnt = periodMillisecond / PERIOD_STEP_10_MINUTE;
        } else {
            return null;
        }

        byte[] keyIndex = new byte[2];
        keyIndex[0] = (byte) (appKeyIndex & 0xFF);
        keyIndex[1] = (byte) ((appKeyIndex >> 8) & 0xFF);

        return new byte[]{keyIndex[0], (byte) (((keyIndex[1] & 0b1111) | (CREDENTIAL_FLAG & 0b1) << 4) | ((RFU & 0b111) << 5)), (byte) (TTL & 0xFF), (byte) ((cnt & MAX_STEP_VALUE) | ((step & 0b11) << 6)), TRANSMIT};
    }
}
