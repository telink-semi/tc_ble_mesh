/********************************************************************************************************
 * @file ModelPublicationGetMessage.java
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Model Publication Get is an acknowledged message used to get the publish address and parameters of an outgoing message that originates from a model.
 * The response to a Config Model Publication Get message is a Config Model Publication Status message.
 *
 * @see ModelPublicationStatusMessage
 */
public class ModelPublicationGetMessage extends ConfigMessage {


    /**
     * Address of the element
     * 16 bits
     */
    public int elementAddress;

    /**
     * 16 or 32 bits
     */
    public int modelId;

    /**
     * if sig model
     * #modelId
     */
    public boolean sig = true;

    public static ModelPublicationGetMessage getSimple(int destinationAddress, int elementAddress, int modelId) {
        ModelPublicationGetMessage message = new ModelPublicationGetMessage(destinationAddress);
        message.elementAddress = elementAddress;
        message.modelId = modelId;
        return message;
    }

    public ModelPublicationGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_MODEL_PUB_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_PUB_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        int len = sig ? 4 : 6;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        bf.putShort((short) elementAddress);
        if (sig) {
            bf.putShort((short) modelId);
        } else {
            bf.putInt(modelId);
        }
        return bf.array();
    }

}
