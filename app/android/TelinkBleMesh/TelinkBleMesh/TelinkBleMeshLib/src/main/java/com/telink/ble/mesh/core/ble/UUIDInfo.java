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


}
