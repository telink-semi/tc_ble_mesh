/********************************************************************************************************
 * @file SensorCadenceGetMessage.java
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
package com.telink.ble.mesh.core.message.sensor;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteOrder;

/**
 * This class represents a message for getting sensor cadence.
 * It extends the GenericMessage class.
 * It contains the index of the scene.
 * It provides methods for creating a simple message, setting the index, and getting the response opcode and opcode.
 */
public class SensorCadenceGetMessage extends GenericMessage {
    // Property ID
    private int propertyID;

    /**
     * Creates and returns a simple scheduler action get message.
     *
     * @param address     The destination address.
     * @param appKeyIndex The application key index.
     * @param propertyID  The index of the scheduler.
     * @return The scheduler action get message.
     */
    public static SensorCadenceGetMessage getSimple(int address, int appKeyIndex, int propertyID) {
        SensorCadenceGetMessage message = new SensorCadenceGetMessage(address, appKeyIndex);
        message.propertyID = propertyID;
        message.setResponseMax(1);
        return message;
    }

    /**
     * Constructs a scheduler action get message with the given destination address and application key index.
     *
     * @param destinationAddress The destination address.
     * @param appKeyIndex        The application key index.
     */
    public SensorCadenceGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the response opcode for the scheduler action get message.
     *
     * @return The response opcode.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.SENSOR_CANDECE_STATUS.value;
    }

    /**
     * Gets the opcode for the scheduler action get message.
     *
     * @return The opcode.
     */
    @Override
    public int getOpcode() {
        return Opcode.SENSOR_CANDECE_GET.value;
    }

    /**
     * Gets the parameters for the scheduler action get message.
     *
     * @return The parameters.
     */
    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(propertyID, 2, ByteOrder.LITTLE_ENDIAN);
    }

    public void setPropertyID(int propertyID) {
        this.propertyID = propertyID;
    }
}