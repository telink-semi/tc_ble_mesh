/********************************************************************************************************
 * @file FirmwareUpdateInfoGetMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;

public class FirmwareUpdateInfoGetMessage extends UpdatingMessage {

    /**
     * Index of the first requested entry from the Firmware Information List state
     * 1 byte
     */
    private byte firstIndex;

    /**
     * Maximum number of entries that the server includes in a Firmware Update Information Status message
     * 1 byte
     */
    private byte entriesLimit;


    public static FirmwareUpdateInfoGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateInfoGetMessage message = new FirmwareUpdateInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.firstIndex = 0;
        message.entriesLimit = 1;
        return message;
    }

    public FirmwareUpdateInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{firstIndex, entriesLimit};
    }

    public void setFirstIndex(byte firstIndex) {
        this.firstIndex = firstIndex;
    }

    public void setEntriesLimit(byte entriesLimit) {
        this.entriesLimit = entriesLimit;
    }
}
