/********************************************************************************************************
 * @file LinkCloseMessage.java
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

import com.telink.ble.mesh.core.message.Opcode;


public class LinkCloseMessage extends RemoteProvisionMessage {


    public static final byte REASON_SUCCESS = 0x00;

    public static final byte REASON_PROHIBITED = 0x01;

    public static final byte REASON_FAIL = 0x02;

    /**
     * 1 byte
     */
    private byte reason;

    public static LinkCloseMessage getSimple(int destinationAddress, int rspMax, byte reason) {
        LinkCloseMessage message = new LinkCloseMessage(destinationAddress);
        message.setResponseMax(rspMax);
        message.reason = reason;
        return message;
    }

    public LinkCloseMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_LINK_CLOSE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.REMOTE_PROV_LINK_STS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{reason};
    }

    public void setReason(byte reason) {
        this.reason = reason;
    }
}
