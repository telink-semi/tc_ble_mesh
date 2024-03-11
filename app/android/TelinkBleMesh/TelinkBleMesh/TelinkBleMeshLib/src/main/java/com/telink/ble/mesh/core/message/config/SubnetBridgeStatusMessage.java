/********************************************************************************************************
 * @file SubnetBridgeStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/9/10.
 */

/**
 * This class represents a status message for the subnet bridge functionality.
 * It extends the StatusMessage class and implements the Parcelable interface for easy serialization.
 * The subnet bridge state is represented by a single byte, where 0x00 indicates that the functionality is disabled,
 * and 0x01 indicates that it is enabled.
 */
public class SubnetBridgeStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * 0x00	Subnet bridge functionality is disabled.
     * 0x01	Subnet bridge functionality is enabled.
     */
    private byte subnetBridgeState;

    /**
     * Default constructor for SubnetBridgeStatusMessage.
     */
    public SubnetBridgeStatusMessage() {
    }

    /**
     * Constructor for SubnetBridgeStatusMessage that initializes the subnet bridge state from a Parcel.
     *
     * @param in The Parcel from which to read the subnet bridge state.
     */
    protected SubnetBridgeStatusMessage(Parcel in) {
        subnetBridgeState = in.readByte();
    }

    /**
     * Creator for SubnetBridgeStatusMessage that is used to create new instances from a Parcel.
     */
    public static final Creator<SubnetBridgeStatusMessage> CREATOR = new Creator<SubnetBridgeStatusMessage>() {
        @Override
        public SubnetBridgeStatusMessage createFromParcel(Parcel in) {
            return new SubnetBridgeStatusMessage(in);
        }

        @Override
        public SubnetBridgeStatusMessage[] newArray(int size) {
            return new SubnetBridgeStatusMessage[size];
        }
    };

    /**
     * Parses the subnet bridge state from a byte array.
     *
     * @param params The byte array containing the subnet bridge state.
     */
    @Override
    public void parse(byte[] params) {
        subnetBridgeState = params[0];
    }

    /**
     * Returns the subnet bridge state.
     *
     * @return The subnet bridge state.
     */
    public byte getSubnetBridgeState() {
        return subnetBridgeState;
    }

    /**
     * Returns the bitmask describing the contents of the Parcelable.
     *
     * @return The bitmask describing the contents of the Parcelable.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the subnet bridge state to a Parcel.
     *
     * @param dest  The Parcel to which the subnet bridge state should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(subnetBridgeState);
    }
}