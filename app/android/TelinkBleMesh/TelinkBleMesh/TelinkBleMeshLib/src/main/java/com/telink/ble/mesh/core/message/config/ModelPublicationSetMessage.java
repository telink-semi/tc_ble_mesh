/********************************************************************************************************
 * @file ModelPublicationSetMessage.java
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
import com.telink.ble.mesh.entity.ModelPublication;

/**
 * The Config Model Publication Set is an acknowledged message used to set the Model Publication state of an outgoing message that originates from a model.
 * <p>
 * The response to a Config Model Publication Set message is a Config Model Publication Status message.
 * {@link ModelPublicationStatusMessage}
 * <p>
 * The Config Model Publication Set Message class represents an outgoing message used to set the Model Publication state of a model.
 * It is used to configure the publication parameters for a specific model on a destination address.
 * The response to this message is a Config Model Publication Status message.
 * Please refer to the ModelPublicationStatusMessage class for more information.
 */
public class ModelPublicationSetMessage extends ConfigMessage {

    private ModelPublication modelPublication;

    /**
     * Constructs a ModelPublicationSetMessage with the specified destination address and model publication parameters.
     *
     * @param destinationAddress The destination address of the message.
     * @param modelPublication   The model publication parameters to be set.
     */
    public ModelPublicationSetMessage(int destinationAddress, ModelPublication modelPublication) {
        super(destinationAddress);
        this.modelPublication = modelPublication;
        this.responseOpcode = Opcode.CFG_MODEL_PUB_STATUS.value;
        this.responseMax = 1;
    }

    /**
     * Constructs a ModelPublicationSetMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public ModelPublicationSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the Model Publication Set message.
     *
     * @return The opcode value for the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_MODEL_PUB_SET.value;
    }

    /**
     * Returns the response opcode value for the Model Publication Set message.
     *
     * @return The response opcode value for the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_PUB_STATUS.value;
    }

    /**
     * Returns the byte array representation of the model publication parameters.
     *
     * @return The byte array representation of the model publication parameters.
     */
    @Override
    public byte[] getParams() {
        return modelPublication.toBytes();
    }

    /**
     * Sets the model publication parameters for the message.
     *
     * @param modelPublication The model publication parameters to be set.
     */
    public void setModelPublication(ModelPublication modelPublication) {
        this.modelPublication = modelPublication;
    }
}