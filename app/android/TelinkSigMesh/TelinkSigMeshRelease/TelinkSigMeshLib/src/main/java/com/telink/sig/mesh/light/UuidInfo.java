/********************************************************************************************************
 * @file UuidInfo.java
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

import java.util.UUID;

/**
 * Created by kee on 2018/7/19.
 */

public class UuidInfo {

    public static final UUID SERVICE_UUID_OTA = UUID.fromString("00010203-0405-0607-0809-0a0b0c0d1912");
    public static final UUID CHARACTERISTIC_UUID_OTA = UUID.fromString("00010203-0405-0607-0809-0a0b0c0d2b12");

    public static final UUID UNUSED_SERVICE_UUID = UUID.fromString("00007fdd-0000-1000-8000-00805f9b34fb");

    public static final UUID PROVISION_SERVICE_UUID = UUID.fromString("00001827-0000-1000-8000-00805f9b34fb");
    public static final UUID PB_IN_CHARACTERISTIC_UUID = UUID.fromString("00002adb-0000-1000-8000-00805f9b34fb");
    public static final UUID PB_OUT_CHARACTERISTIC_UUID = UUID.fromString("00002adc-0000-1000-8000-00805f9b34fb");

    public static final UUID PROXY_SERVICE_UUID = UUID.fromString("00001828-0000-1000-8000-00805f9b34fb");
    public static final UUID PROXY_IN_CHARACTERISTIC_UUID = UUID.fromString("00002add-0000-1000-8000-00805f9b34fb");
    public static final UUID PROXY_OUT_CHARACTERISTIC_UUID = UUID.fromString("00002ade-0000-1000-8000-00805f9b34fb");

    public static final UUID SERVICE_UUID_ONLINE_STATUS = UUID.fromString("00010203-0405-0607-0809-0a0b0c0d1a10");
    public static final UUID CHARACTERISTIC_UUID_ONLINE_STATUS = UUID.fromString("00010203-0405-0607-0809-0a0b0c0d1a11");

    public static final UUID CFG_DESCRIPTOR_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

}
