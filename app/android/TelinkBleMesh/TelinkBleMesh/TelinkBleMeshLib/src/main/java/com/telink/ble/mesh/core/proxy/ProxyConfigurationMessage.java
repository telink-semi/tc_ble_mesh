/********************************************************************************************************
 * @file ProxyConfigurationMessage.java
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
 * Created by kee on 2019/8/26.
 */

public abstract class ProxyConfigurationMessage {


    /**
     * Set Filter Type
     * Sent by a Proxy Client to set the proxy filter typeValue.
     */
    public static final byte OPCODE_SET_FILTER_TYPE = 0x00;

    /**
     * Add Addresses To Filter
     * Sent by a Proxy Client to add addresses to the proxy filter list.
     */
    public static final byte OPCODE_ADD_ADDRESS = 0x01;

    /**
     * Remove Addresses From Filter
     * Sent by a Proxy Client to remove addresses from the proxy filter list.
     */
    public static final byte OPCODE_REMOVE_ADDRESS = 0x02;

    /**
     * Filter Status
     * Acknowledgment by a Proxy Server to a Proxy Client to report the status of the proxy filter list.
     */
    public static final byte OPCODE_FILTER_STATUS = 0x03;


    public abstract byte getOpcode();

    public abstract byte[] toByteArray();

}
