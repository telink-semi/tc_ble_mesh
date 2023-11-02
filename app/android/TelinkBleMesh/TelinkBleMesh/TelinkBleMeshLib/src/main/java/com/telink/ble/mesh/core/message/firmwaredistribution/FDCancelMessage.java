/********************************************************************************************************
 * @file FDCancelMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

/**
 * The Firmware Distribution Cancel message is an acknowledged message
 * sent by a Firmware Distribution Client to stop the firmware image distribution from a Firmware Distribution Server.
 * The response to a Firmware Distribution Cancel message is a Firmware Distribution Status message.
 */
public class FDCancelMessage extends UpdatingMessage {

    private boolean ack = true; // Flag indicating if an acknowledgement is expected 

    /**
     * Constructor for the FDCancelMessage class.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public FDCancelMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDCancelMessage with default values.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     * @return A simple FDCancelMessage object.
     */
    public static FDCancelMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDCancelMessage message = new FDCancelMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Returns the opcode value for the FDCancelMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_CANCEL.value;
    }

    /**
     * Returns the acknowledgement flag for the FDCancelMessage.
     *
     * @return The acknowledgement flag.
     */
    public boolean isAck() {
        return ack;
    }

    /**
     * Sets the acknowledgement flag for the FDCancelMessage.
     *
     * @param ack The acknowledgement flag.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Returns the response opcode value for the FDCancelMessage.
     * If the acknowledgement flag is true, it returns the FD_STATUS opcode value.
     * Otherwise, it returns the OPCODE_INVALID value.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.FD_STATUS.value : MeshMessage.OPCODE_INVALID;
    }
}
