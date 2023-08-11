/********************************************************************************************************
 * @file FirmwareUpdateCancelMessage.java
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
package com.telink.ble.mesh.core.message.directforwarding;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class DirectedControlSetMessage extends ConfigMessage {

    /**
     * 16 bits
     */
    public int netKeyIndex;

    /**
     * 8 bits
     */
    public byte directedForwarding;

    /**
     * 8 bits
     */
    public byte directedRelay;

    /**
     * 8 bits
     */
    private byte directedProxy;

    /**
     * 8 bits
     */
    private byte directedProxyUseDirectedDefault;

    /**
     * 8 bits
     */
    public byte directedFriend;

    public static DirectedControlSetMessage getSimple(int destinationAddress, int netKeyIndex) {
        DirectedControlSetMessage message = new DirectedControlSetMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    public static DirectedControlSetMessage getDFEnable(int destinationAddress, int netKeyIndex, boolean enableDirectForwarding) {
        DirectedControlSetMessage message = new DirectedControlSetMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        message.directedForwarding = (byte) (enableDirectForwarding ? 1 : 0);
        return message;
    }

    public DirectedControlSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.DIRECTED_CONTROL_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.DIRECTED_CONTROL_STATUS.value;
    }

    public void setDirectedProxy(byte directedProxy) {
        this.directedProxy = directedProxy;
        this.directedProxyUseDirectedDefault = directedProxy;
    }

    public void setDirectedProxyUseDirectedDefault(byte directedProxyUseDirectedDefault) {
        this.directedProxyUseDirectedDefault = directedProxyUseDirectedDefault;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) netKeyIndex)
                .put(directedForwarding)
                .put(directedRelay)
                .put(directedProxy)
                .put(directedProxyUseDirectedDefault)
                .put(directedFriend).array();
    }
}
