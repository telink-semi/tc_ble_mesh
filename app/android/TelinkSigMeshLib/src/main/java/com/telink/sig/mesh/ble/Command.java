/********************************************************************************************************
 * @file     Command.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.ble;

import com.telink.sig.mesh.util.Arrays;

import java.util.UUID;

public class Command {

    public UUID serviceUUID;
    public UUID characteristicUUID;
    public UUID descriptorUUID;
    public CommandType type;
    public byte[] data;
    public Object tag;
    public int flag;
    public int delay;

    public Command() {
        this(null, null, CommandType.WRITE);
    }

    public Command(UUID serviceUUID, UUID characteristicUUID, CommandType type) {
        this(serviceUUID, characteristicUUID, type, null);
    }

    public Command(UUID serviceUUID, UUID characteristicUUID, CommandType type,
                   byte[] data) {
        this(serviceUUID, characteristicUUID, type, data, null);
    }

    public Command(UUID serviceUUID, UUID characteristicUUID, CommandType type,
                   byte[] data, Object tag) {

        this.serviceUUID = serviceUUID;
        this.characteristicUUID = characteristicUUID;
        this.type = type;
        this.data = data;
        this.tag = tag;
    }

    public static Command newInstance() {
        return new Command();
    }

    public void clear() {
        this.serviceUUID = null;
        this.characteristicUUID = null;
        this.descriptorUUID = null;
        this.data = null;
    }

    @Override
    public String toString() {
        String d = "";

        if (data != null)
            d = Arrays.bytesToHexString(this.data, ",");

        return "{ tag : " + this.tag + ", type : " + this.type
                + " CHARACTERISTIC_UUID_WRITE :" + characteristicUUID.toString() + " data: " + d + " delay :" + delay + "}";
    }

    public enum CommandType {
        READ, READ_DESCRIPTOR, WRITE, WRITE_NO_RESPONSE, WRITE_DESCRIPTOR, ENABLE_NOTIFY, DISABLE_NOTIFY
    }
}
