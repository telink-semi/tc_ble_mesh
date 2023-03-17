/********************************************************************************************************
 * @file ProxyPDU.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.proxy;

/**
 * Created by kee on 2019/7/18.
 */

public abstract class ProxyPDU {

    /**
     * defines if message is segment
     * 2 bits
     */
    public byte sar;

    /**
     * 6 bits
     * defines data field content
     */
    public byte type;

    public byte[] data;

    /**
     * get proxy pdu typeValue and SAR by ' data[0] & BITS_* '
     */
    public static final int BITS_TYPE = 0b00111111;

    public static final int BITS_SAR = 0b11000000;


    /**
     * complete message
     */
    public static final byte SAR_COMPLETE = 0b00;

    /**
     * segment message
     */
    public static final byte SAR_SEG_FIRST = 0b01 << 6;

    public static final byte SAR_SEG_CONTINUE = (byte) (0b10 << 6);

    public static final byte SAR_SEG_LAST = (byte) (0b11 << 6);


    /**
     * PDU typeValue
     */

    public static final byte TYPE_NETWORK_PDU = 0x00;

    public static final byte TYPE_MESH_BEACON = 0x01;

    public static final byte TYPE_PROXY_CONFIGURATION = 0x02;

    public static final byte TYPE_PROVISIONING_PDU = 0x03;
}
