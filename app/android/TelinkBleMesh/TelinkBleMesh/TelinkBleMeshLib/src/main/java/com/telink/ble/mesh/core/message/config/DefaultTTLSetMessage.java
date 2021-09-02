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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/12.
 */

public class DefaultTTLSetMessage extends ConfigMessage {

    /**
     * New Default TTL value, difference with {@link com.telink.ble.mesh.core.message.MeshMessage#ttl}
     */
    public byte newTTL;

    public DefaultTTLSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static DefaultTTLSetMessage getSimple(int destinationAddress, byte newTTL) {
        DefaultTTLSetMessage instance = new DefaultTTLSetMessage(destinationAddress);
        instance.newTTL = newTTL;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_DEFAULT_TTL_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_DEFAULT_TTL_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{newTTL};
    }
}
