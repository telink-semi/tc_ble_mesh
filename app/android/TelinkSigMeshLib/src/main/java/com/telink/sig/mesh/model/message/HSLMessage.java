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

/**
 * Created by kee on 2019/2/25.
 */

public class HSLMessage implements ModelMessage {

    /**
     * 2 bytes
     */
    public int lightness;

    /**
     * 2 bytes
     */
    public int hue;

    /**
     * 2 bytes
     */
    public int saturation;

    /**
     * Transaction Identifier
     */
    public byte tid;

    /**
     * @see TransitionTime#getValue()
     */
    public byte transTime;

    /**
     * unit 5ms
     */
    public byte delay;

    public static HSLMessage createInstance(int hue, int saturation, int lightness) {
        HSLMessage instance = new HSLMessage();
        instance.hue = hue;
        instance.saturation = saturation;
        instance.lightness = lightness;
        instance.tid = 0;
        instance.transTime = 0;
        instance.delay = 0;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        return new byte[]{
                (byte) (this.lightness), (byte) (this.lightness >> 8),
                (byte) (this.hue), (byte) (this.hue >> 8),
                (byte) (this.saturation), (byte) (this.saturation >> 8),
                this.tid,
                this.transTime,
                this.delay};
    }
}
