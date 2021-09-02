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
 * The Config Key Refresh Phase Set is an acknowledged message used to set the Key Refresh Phase state of the identified network key
 */

public class KeyRefreshPhaseSetMessage extends ConfigMessage {

    public int netKeyIndex;

    /**
     * New Key Refresh Phase Transition
     */
    public byte transition;

    public KeyRefreshPhaseSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static KeyRefreshPhaseSetMessage getSimple(int destinationAddress, byte gattProxy) {
        KeyRefreshPhaseSetMessage instance = new KeyRefreshPhaseSetMessage(destinationAddress);
        instance.transition = gattProxy;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_KEY_REFRESH_PHASE_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_KEY_REFRESH_PHASE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN).putShort((short) netKeyIndex)
                .put(transition).array();
    }
}
