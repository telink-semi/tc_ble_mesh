/********************************************************************************************************
 * @file ModelAppBindMessage.java
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Model App Bind is an acknowledged message used to bind an AppKey to a model.
 * <p>
 * The response to a Config Model App Bind message is a Config Model App Status message.
 * {@link ModelAppStatusMessage}
 */

public class ModelAppBindMessage extends ConfigMessage {

    /**
     * Address of the element
     * 2 bytes
     */
    private int elementAddress;

    /**
     * Index of the AppKey
     * 2 bytes, really 12 bit used
     */
    private int appKeyIndex;

    /**
     * SIG Model ID or Vendor Model ID
     * if SIG: 2 bytes
     * else : 4 bytes
     */
    private int modelIdentifier;

    private boolean isSigModel = true;

    public ModelAppBindMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public void setElementAddress(int elementAddress) {
        this.elementAddress = elementAddress;
    }

    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    public void setModelIdentifier(int modelIdentifier) {
        this.modelIdentifier = modelIdentifier;
    }

    public void setSigModel(boolean sigModel) {
        isSigModel = sigModel;
    }

    @Override
    public int getOpcode() {
        return Opcode.MODE_APP_BIND.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.MODE_APP_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        // check if sig model or vendor model
//        boolean isSigModel = isSigModel(this.modelIdentifier);
        int bufferSize = isSigModel ? 6 : 8;
        ByteBuffer paramsBuffer = ByteBuffer.allocate(bufferSize).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) this.elementAddress)
                .putShort((short) this.appKeyIndex);
        if (isSigModel) {
            paramsBuffer.putShort((short) this.modelIdentifier);
        } else {
            paramsBuffer.putInt(this.modelIdentifier);
        }
        return paramsBuffer.array();
    }
}
