/********************************************************************************************************
 * @file LowerTransportPDU.java
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
package com.telink.ble.mesh.core.networking.transport.lower;

import com.telink.ble.mesh.core.networking.NetworkingPDU;

/**
 * big endian
 * transport: access message, control message
 * Created by kee on 2019/7/22.
 */
public abstract class LowerTransportPDU implements NetworkingPDU {

    public static int TYPE_UNSEGMENTED_ACCESS_MESSAGE = 0xb00;

    public static int TYPE_SEGMENTED_ACCESS_MESSAGE = 0xb01;

    public static int TYPE_UNSEGMENTED_CONTROL_MESSAGE = 0xb10;

    public static int TYPE_SEGMENTED_CONTROL_MESSAGE = 0xb11;


    public static int SEG_TYPE_UNSEGMENTED = 0;

    public static int SEG_TYPE_SEGMENTED = 1;

    protected int seg;

    /**
     * get pdu typeValue
     *
     * @return PDU typeValue
     */
    public abstract int getType();

    /**
     * segmented state, determined by seg bit significant
     *
     * @return if is a segment pdu
     */
    public abstract boolean segmented();


}
