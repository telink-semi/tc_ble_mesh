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
 * <p>
 * The Config Model Publication Get message is used to request the publish address and parameters of an outgoing message that originates from a model.
 * This message is sent to a destination address and expects a response in the form of a Config Model Publication Status message.
 *
 * @see ModelPublicationStatusMessage
 */
public class ModelPublicationGetMessage extends ConfigMessage {
    /**
     * The address of the element.
     * It is represented by 16 bits.
     */
    public int elementAddress;
    /**
     * The ID of the model.
     * It can be represented by either 16 or 32 bits.
     */
    public int modelId;
    /**
     * Specifies whether the model is a SIG model.
     * If true, the model is a SIG model.
     * If false, the model is a vendor model.
     */
    public boolean sig = true;

    /**
     * Creates a simple ModelPublicationGetMessage object.
     *
     * @param destinationAddress The destination address of the message.
     * @param elementAddress     The address of the element.
     * @param modelId            The ID of the model.
     * @return A ModelPublicationGetMessage object.
     */
    public static ModelPublicationGetMessage getSimple(int destinationAddress, int elementAddress, int modelId) {
        ModelPublicationGetMessage message = new ModelPublicationGetMessage(destinationAddress);
        message.elementAddress = elementAddress;
        message.modelId = modelId;
        return message;
    }

    /**
     * Constructs a ModelPublicationGetMessage object with the given destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public ModelPublicationGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_MODEL_PUB_GET.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_PUB_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters as a byte array.
     */
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