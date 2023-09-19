/********************************************************************************************************
 * @file FDUploadOOBStartMessage.java
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
 * The Firmware Distribution Upload OOB Start message is an acknowledged message
 * sent by a Firmware Distribution Client to start a firmware image upload to a Firmware Distribution Server using an out-of-band mechanism.
 */
public class FDUploadOOBStartMessage extends UpdatingMessage {

    /**
     * Upload URI Length
     * Length of the Upload URI field
     * 1 byte
     */
    public int uploadURILength;

    /**
     * Upload URI
     * URI for the firmware image source
     * 1 to 255 bytes
     */
    public int uploadURI;

    /**
     * Upload Firmware ID
     * The Firmware ID value used to generate the URI query string
     * Variable length
     */
    public byte[] uploadFirmwareID;

    public FDUploadOOBStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    public static FDUploadOOBStartMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDUploadOOBStartMessage message = new FDUploadOOBStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    @Override
    public int getOpcode() {
        return Opcode.FD_UPLOAD_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FD_UPLOAD_STATUS.value;
    }


}
