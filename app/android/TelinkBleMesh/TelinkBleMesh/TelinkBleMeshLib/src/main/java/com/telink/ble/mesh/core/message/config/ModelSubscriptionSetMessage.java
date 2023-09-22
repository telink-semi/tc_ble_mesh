/********************************************************************************************************
 * @file ModelSubscriptionSetMessage.java
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
 * device grouping
 * composite of [Config Model Subscription Add] and [Config Model Subscription Delete]
 * <p>
 * This class represents a message for setting model subscriptions in a device grouping.
 * It is a composite of [Config Model Subscription Add] and [Config Model Subscription Delete] messages.
 * The response to a Config Model Subscription Delete message is a Config Model Subscription Status message.
 * See {@link ModelSubscriptionStatusMessage} for more information.
 */
public class ModelSubscriptionSetMessage extends ConfigMessage {
    private static final int PARAM_LEN_SIG = 6;
    private static final int PARAM_LEN_VENDOR = 8;
    public static final int MODE_ADD = 0;
    public static final int MODE_DELETE = 1;
    private int mode = MODE_ADD;
    private int elementAddress;
    /**
     * The group address for the model subscription.
     */
    private int address;
    /**
     * The identifier of the model.
     * The length of the identifier is determined by whether it is a SIG or vendor model.
     */
    private int modelIdentifier;
    /**
     * Indicates whether the model is a SIG model or a vendor model.
     */
    private boolean isSig = true;

    /**
     * Creates a new instance of ModelSubscriptionSetMessage with the given destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public ModelSubscriptionSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Creates a simple ModelSubscriptionSetMessage with the given parameters.
     *
     * @param destinationAddress The destination address of the message.
     * @param mode               The mode of the message (MODE_ADD or MODE_DELETE).
     * @param elementAddress     The address of the element.
     * @param groupAddress       The group address.
     * @param modelId            The identifier of the model.
     * @param isSig              Indicates whether the model is a SIG model or a vendor model.
     * @return A ModelSubscriptionSetMessage with the specified parameters.
     */
    public static ModelSubscriptionSetMessage getSimple(int destinationAddress, int mode, int elementAddress, int groupAddress, int modelId, boolean isSig) {
        ModelSubscriptionSetMessage message = new ModelSubscriptionSetMessage(destinationAddress);
        message.mode = mode;
        message.elementAddress = elementAddress;
        message.address = groupAddress;
        message.modelIdentifier = modelId;
        message.isSig = isSig;
        return message;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return mode == MODE_ADD ? Opcode.CFG_MODEL_SUB_ADD.value : Opcode.CFG_MODEL_SUB_DEL.value;
    }

    /**
     * Gets the opcode of the response message.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_SUB_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        if (isSig) {
            return ByteBuffer.allocate(PARAM_LEN_SIG).order(ByteOrder.LITTLE_ENDIAN)
                    .putShort((short) elementAddress)
                    .putShort((short) address)
                    .putShort((short) modelIdentifier)
                    .array();
        } else {
            return ByteBuffer.allocate(PARAM_LEN_VENDOR).order(ByteOrder.LITTLE_ENDIAN)
                    .putShort((short) elementAddress)
                    .putShort((short) address)
                    .putInt(modelIdentifier)
                    .array();
        }
    }

    /**
     * Sets the mode of the message.
     *
     * @param mode The mode of the message (MODE_ADD or MODE_DELETE).
     */
    public void setMode(int mode) {
        this.mode = mode;
    }

    /**
     * Sets the address of the element.
     *
     * @param elementAddress The address of the element.
     */
    public void setElementAddress(int elementAddress) {
        this.elementAddress = elementAddress;
    }

    /**
     * Sets the group address for the model subscription.
     *
     * @param address The group address.
     */
    public void setAddress(int address) {
        this.address = address;
    }

    /**
     * Sets the identifier of the model.
     *
     * @param modelIdentifier The identifier of the model.
     */
    public void setModelIdentifier(int modelIdentifier) {
        this.modelIdentifier = modelIdentifier;
    }

    /**
     * Sets whether the model is a SIG model or a vendor model.
     *
     * @param sig Indicates whether the model is a SIG model or a vendor model.
     */
    public void setSig(boolean sig) {
        isSig = sig;
    }
}