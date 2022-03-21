/********************************************************************************************************
 * @file RPStatusMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/8/20.
 */

public abstract class RPStatusMessage extends StatusMessage {

    /**
     * defines status codes for Remote Provisioning Server messages that contain a status code.
     */

    public static final byte CODE_SUCCESS = 0x00;

    public static final byte CODE_SCANNING_CANNOT_START = 0x01;

    public static final byte CODE_INVALID_STATE = 0x02;

    public static final byte CODE_LIMITED_RESOURCES = 0x03;

    public static final byte CODE_LINK_CANNOT_OPEN = 0x04;

    public static final byte CODE_LINK_OPEN_FAILED = 0x05;

    public static final byte CODE_LINK_CLOSED_BY_DEVICE = 0x06;

    public static final byte CODE_LINK_CLOSED_BY_SERVER = 0x07;

    public static final byte CODE_LINK_CLOSED_BY_CLIENT = 0x08;

    public static final byte CODE_LINK_CLOSED_AS_CANNOT_RECEIVE_PDU = 0x09;

    public static final byte CODE_LINK_CLOSED_AS_CANNOT_SEND_PDU = 0x0A;

    public static final byte CODE_LINK_CLOSED_AS_CANNOT_DELIVER_PDU_REPORT = 0x0B;

    public static final byte CODE_LINK_CLOSED_AS_CANNOT_DELIVER_PDU_OUTBOUND_REPORT = 0x0C;

}
