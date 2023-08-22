/********************************************************************************************************
 * @file PrivateBeaconSetMessage.java
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
package com.telink.ble.mesh.core.message.privatebeacon;

import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * A PRIVATE_BEACON_SET message is an acknowledged message used to set the Private Beacon state and the Random Update Interval Steps state of a node.
 * The response to a PRIVATE_BEACON_SET message is a PRIVATE_BEACON_STATUS message.
 */

public class PrivateBeaconSetMessage extends ConfigMessage {

    /**
     * 1 byte
     * New Private Beacon state
     */
    public byte privateBeacon;

    /**
     * 2 bytes
     * New Random Update Interval Steps state (optional)
     */
    public int randomUpdateIntervalSteps;

    public boolean isComplete = false;

    public PrivateBeaconSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        if (!isComplete) {
            return new byte[]{privateBeacon};
        } else {
            return ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                    .put(privateBeacon).putShort((short) randomUpdateIntervalSteps).array();
        }
    }


}
