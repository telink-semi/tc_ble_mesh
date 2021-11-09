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
 * The Config Friend Set is an acknowledged message used to set the Friend state of a node.
 * <p>
 * The response to a Config Friend Get message is a Config Friend Status message.
 * * {@link FriendStatusMessage}
 */

public class FriendSetMessage extends ConfigMessage {

    /**
     * New Friend state
     */
    public byte friend;

    public FriendSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static FriendSetMessage getSimple(int destinationAddress, byte friend) {
        FriendSetMessage instance = new FriendSetMessage(destinationAddress);
        instance.friend = friend;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_FRIEND_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_FRIEND_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{friend};
    }
}
