/********************************************************************************************************
 * @file BeaconSetMessage.java
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
 * The Config Beacon Set is an acknowledged message used to set the Secure Network Beacon state of a node
 */
public class BeaconSetMessage extends ConfigMessage {

    /**
     * 0x00 The node is not broadcasting a Secure Network beacon
     * 0x01 The node is broadcasting a Secure Network beacon
     */
    public byte beacon;

    public BeaconSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public static BeaconSetMessage getSimple(int destinationAddress, byte beacon) {
        BeaconSetMessage instance = new BeaconSetMessage(destinationAddress);
        instance.beacon = beacon;
        return instance;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_BEACON_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_BEACON_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{beacon};
    }


}
