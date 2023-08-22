/********************************************************************************************************
 * @file FDStartMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Firmware Distribution Start message is an acknowledged message sent by a Firmware Distribution Client to start the firmware image distribution to the Updating nodes in the Distribution Receivers List.
 * The response to a Firmware Distribution Start message is a Firmware Distribution Status message.
 *
 * @see
 */
public class FDStartMessage extends UpdatingMessage {

    /**
     * Distribution AppKey Index
     * Index of the application key used in a firmware image distribution
     * 16 bits
     */
    public int distributionAppKeyIndex;

    /**
     * Distribution TTL
     * Time To Live value used in a firmware image distribution
     * 8 bits
     */
    public int distributionTTL;

    /**
     * Distribution Timeout Base
     * Used to compute the timeout of the firmware image distribution
     * 16 bits
     */
    public int distributionTimeoutBase;

    /**
     * Distribution Transfer Mode
     * Mode of the transfer
     * 2 bits
     */
    public int distributionTransferMode;

    /**
     * Update Policy
     * Firmware update policy
     * 1 bit
     */
    public int updatePolicy;


    /**
     * Reserved for Future Use
     * 5 bits
     */
    public int RFU = 0;

    /**
     * Distribution Firmware Image Index
     * Index of the firmware image in the Firmware Images List state to use during firmware image distribution
     * 16 bits
     */
    public int distributionImageIndex;

    /**
     * Distribution Multicast Address
     * Multicast address used in a firmware image distribution
     * 16 or 128 bits
     * 16 bit group address currently
     */
    public int distributionMulticastAddress;


    public FDStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    public static FDStartMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDStartMessage message = new FDStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    @Override
    public int getOpcode() {
        return Opcode.FD_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FD_STATUS.value;
    }

    @Override
    public byte[] getParams() {

        byte modePolicyType = (byte) ((distributionTransferMode & 0b11)
                | ((updatePolicy & 0b1) << 2));

        return ByteBuffer.allocate(8 + 2).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) distributionAppKeyIndex)
                .put((byte) distributionTTL)
                .putShort((short) distributionTimeoutBase)
                .put(modePolicyType)
                .putShort((short) distributionImageIndex)
                .putShort((short) distributionMulticastAddress).array();
    }
}
