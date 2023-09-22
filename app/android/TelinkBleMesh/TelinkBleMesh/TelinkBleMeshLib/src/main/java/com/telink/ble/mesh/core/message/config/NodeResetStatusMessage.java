/********************************************************************************************************
 * @file NodeResetStatusMessage.java
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
 * The Config Node Reset Status is an unacknowledged message used to acknowledge that an element has received a Config Node Reset message.
 * The NodeResetStatusMessage class represents an unacknowledged message used to acknowledge that an element has received a Config Node Reset message.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * <p>
 */
public class NodeResetStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Default constructor for the NodeResetStatusMessage class.
     */
    public NodeResetStatusMessage() {
    }

    /**
     * Constructor for the NodeResetStatusMessage class that takes a Parcel as input.
     * This is used for creating a NodeResetStatusMessage object from a Parcel.
     *
     * @param in The Parcel object used to construct the NodeResetStatusMessage.
     */
    protected NodeResetStatusMessage(Parcel in) {
    }

    /**
     * Creator constant for the NodeResetStatusMessage class.
     * Used to create new instances of the NodeResetStatusMessage class from a Parcel.
     */
    public static final Creator<NodeResetStatusMessage> CREATOR = new Creator<NodeResetStatusMessage>() {
        @Override
        public NodeResetStatusMessage createFromParcel(Parcel in) {
            return new NodeResetStatusMessage(in);
        }

        @Override
        public NodeResetStatusMessage[] newArray(int size) {
            return new NodeResetStatusMessage[size];
        }
    };

    /**
     * This method is used to parse the byte array params.
     * It overrides the parse method of the StatusMessage class.
     *
     * @param params The byte array to be parsed.
     */
    @Override
    public void parse(byte[] params) {
    }

    /**
     * This method returns a bitmask indicating the set of special object types
     * marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types marshaled by this Parcelable instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * This method writes the NodeResetStatusMessage object to a Parcel.
     * It overrides the writeToParcel method of the Parcelable interface.
     *
     * @param dest  The Parcel object to which the NodeResetStatusMessage should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
    }
}