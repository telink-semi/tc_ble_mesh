/********************************************************************************************************
 * @file FDApplyMessage.java
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

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;


/**
 * The Firmware Distribution Apply message is an acknowledged message
 * sent from a Firmware Distribution Client to a Firmware Distribution Server to apply the firmware image on the Updating nodes.
 * The response to a Firmware Distribution Apply message is a Firmware Distribution Status message.
 */
public class FDApplyMessage extends UpdatingMessage {

    /**
     * Constructs a FDApplyMessage object with the given destination address and app key index.
     *
     * @param destinationAddress The address of the destination device.
     * @param appKeyIndex        The app key index to be used for the firmware update.
     */
    public FDApplyMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDApplyMessage object with the given destination address and app key index.
     * Sets the maximum number of responses to 1.
     *
     * @param destinationAddress The address of the destination device.
     * @param appKeyIndex        The app key index to be used for the firmware update.
     * @return The created FDApplyMessage object.
     */
    public static FDApplyMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDApplyMessage message = new FDApplyMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Gets the opcode value for the FD_APPLY message.
     *
     * @return The opcode value for the FD_APPLY message.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_APPLY.value;
    }

    /**
     * Gets the opcode value for the FD_STATUS response message.
     *
     * @return The opcode value for the FD_STATUS response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_STATUS.value;
    }
}