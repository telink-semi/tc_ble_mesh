/********************************************************************************************************
 * @file ProxyPDU.java
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
package com.telink.ble.mesh.core.proxy;

/**
 * This abstract class represents a Proxy Protocol Data Unit (PDU).
 * A PDU is a message that is sent between devices in a network.
 * The ProxyPDU class provides fields and constants for defining the structure and content of a PDU.
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
    public static final int MASK_TYPE = 0b00111111;

    /**
     * used to extract the sar field from the data[0] byte using bitwise AND operation.
     */
    public static final int MASK_SAR = 0b11000000;


    /**
     * complete message
     */
    public static final byte SAR_COMPLETE = 0b00;

    /**
     * segment message
     */
    public static final byte SAR_SEG_FIRST = 0b01 << 6;

    /**
     * represents a segment that is not the first or last segment of a message.
     */
    public static final byte SAR_SEG_CONTINUE = (byte) (0b10 << 6);

    /**
     * represents the last segment of a message.
     */
    public static final byte SAR_SEG_LAST = (byte) (0b11 << 6);


    /**
     * PDU typeValue
     */

    /**
     * represents a PDU for network communication.
     */
    public static final byte TYPE_NETWORK_PDU = 0x00;

    /**
     * represents a PDU for mesh beacon communication.
     */
    public static final byte TYPE_MESH_BEACON = 0x01;

    /**
     * represents a PDU for proxy configuration.
     */
    public static final byte TYPE_PROXY_CONFIGURATION = 0x02;

    /**
     * represents a PDU for provisioning communication.
     */
    public static final byte TYPE_PROVISIONING_PDU = 0x03;
}
