/********************************************************************************************************
 * @file ModelAppBindMessage.java
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
 * The Config Model App Bind Message is used to bind an AppKey to a model in a mesh network.
 * This message is sent to configure the model's application key.
 * <p>
 * The response to this message is a Config Model App Status message.
 *
 * @see ModelAppStatusMessage
 */
public class ModelAppBindMessage extends ConfigMessage {
    /**
     * The address of the element to bind the AppKey to.
     * This is a 2-byte value.
     */
    private int elementAddress;

    /**
     * The index of the AppKey to bind.
     * This is a 2-byte value, but only 12 bits are used.
     */
    private int appKeyIndex;

    /**
     * The SIG Model ID or Vendor Model ID.
     * If it is a SIG Model, it is a 2-byte value.
     * If it is a Vendor Model, it is a 4-byte value.
     */
    private int modelIdentifier;

    /**
     * Flag indicating whether the model is a SIG Model or a Vendor Model.
     * True if it is a SIG Model, false if it is a Vendor Model.
     */
    private boolean isSigModel = true;

    /**
     * Constructs a new ModelAppBindMessage with the specified destination address.
     *
     * @param destinationAddress The address of the destination node.
     */
    public ModelAppBindMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Sets the element address to bind the AppKey to.
     *
     * @param elementAddress The element address.
     */
    public void setElementAddress(int elementAddress) {
        this.elementAddress = elementAddress;
    }

    /**
     * Sets the index of the AppKey to bind.
     *
     * @param appKeyIndex The AppKey index.
     */
    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    /**
     * Sets the model identifier (SIG Model ID or Vendor Model ID).
     *
     * @param modelIdentifier The model identifier.
     */
    public void setModelIdentifier(int modelIdentifier) {
        this.modelIdentifier = modelIdentifier;
    }

    /**
     * Sets the flag indicating whether the model is a SIG Model or a Vendor Model.
     *
     * @param sigModel True if it is a SIG Model, false if it is a Vendor Model.
     */
    public void setSigModel(boolean sigModel) {
        isSigModel = sigModel;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.MODE_APP_BIND.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.MODE_APP_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters as a byte array.
     */
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
