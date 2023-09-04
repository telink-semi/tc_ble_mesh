/********************************************************************************************************
 * @file UUIDInfo.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.core.ble;

import java.util.UUID;

/**
 * bluetooth UUIDs including service UUIDs and characteristic UUIDs
 */

public class UUIDInfo {

    public static final UUID SERVICE_UUID_OTA = UUID.fromString("00010203-0405-0607-0809-0A0B0C0D1912");
    public static final UUID CHARACTERISTIC_UUID_OTA = UUID.fromString("00010203-0405-0607-0809-0A0B0C0D2B12");

    public static final UUID SERVICE_MESH_FLEX = UUID.fromString("00007FDD-0000-1000-8000-00805F9B34FB");

    public static final UUID SERVICE_PROVISION = UUID.fromString("00001827-0000-1000-8000-00805F9B34FB");
    public static final UUID CHARACTERISTIC_PB_IN = UUID.fromString("00002ADB-0000-1000-8000-00805F9B34FB");
    public static final UUID CHARACTERISTIC_PB_OUT = UUID.fromString("00002ADC-0000-1000-8000-00805F9B34FB");

    public static final UUID SERVICE_PROXY = UUID.fromString("00001828-0000-1000-8000-00805F9B34FB");
    public static final UUID CHARACTERISTIC_PROXY_IN = UUID.fromString("00002ADD-0000-1000-8000-00805F9B34FB");
    public static final UUID CHARACTERISTIC_PROXY_OUT = UUID.fromString("00002ADE-0000-1000-8000-00805F9B34FB");

    public static final UUID SERVICE_ONLINE_STATUS = UUID.fromString("00010203-0405-0607-0809-0A0B0C0D1A10");
    public static final UUID CHARACTERISTIC_ONLINE_STATUS = UUID.fromString("00010203-0405-0607-0809-0A0B0C0D1A11");

    public static final UUID DESCRIPTOR_CFG_UUID = UUID.fromString("00002902-0000-1000-8000-00805F9B34FB");

    public static final UUID SERVICE_DEVICE_INFO = UUID.fromString("0000180A-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_FW_VERSION = UUID.fromString("00002A26-0000-1000-8000-00805F9B34FB");



    public static final UUID SERVICE_OTS = UUID.fromString("00001825-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OTS_FEATURE = UUID.fromString("00002ABD-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_NAME = UUID.fromString("00002ABE-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_TYPE = UUID.fromString("00002ABF-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_SIZE = UUID.fromString("00002AC0-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_FIRST_CREATED = UUID.fromString("00002AC1-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_LAST_MODIFIED = UUID.fromString("00002AC2-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_ID = UUID.fromString("00002AC3-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_PROPERTIES = UUID.fromString("00002AC4-0000-1000-8000-00805F9B34FB");

    // object action control point
    public static final UUID CHARACTERISTIC_OACP = UUID.fromString("00002AC5-0000-1000-8000-00805F9B34FB"); //  contains CCC
    // object list control point
    public static final UUID CHARACTERISTIC_OLCP = UUID.fromString("00002AC6-0000-1000-8000-00805F9B34FB"); //  contains CCC

    public static final UUID CHARACTERISTIC_OBJECT_LIST_FILTER = UUID.fromString("00002AC7-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_OBJECT_CHANGED = UUID.fromString("00002AC8-0000-1000-8000-00805F9B34FB"); //  contains CCC

    public static final UUID CHARACTERISTIC_IOS_PSM = UUID.fromString("ABDD3056-28FA-441D-A470-55A75A52553A");

    public static final UUID SERVICE_GENERIC_ATTRIBUTE = UUID.fromString("00001801-0000-1000-8000-00805F9B34FB");

    public static final UUID CHARACTERISTIC_SERVICE_CHANGED = UUID.fromString("00002A05-0000-1000-8000-00805F9B34FB");
}
