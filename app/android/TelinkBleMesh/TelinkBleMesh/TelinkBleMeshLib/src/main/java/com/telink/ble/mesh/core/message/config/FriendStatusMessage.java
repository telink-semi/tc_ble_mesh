/********************************************************************************************************
 * @file FriendStatusMessage.java
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
 * The Config Friend Status is an unacknowledged message used to report the current Friend state of a node
 * The FriendStatusMessage class represents an unacknowledged message used to report the current Friend state of a node.
 * This message is a response to FriendGetMessage and FriendSetMessage.
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_FRIEND_STATUS
 */
public class FriendStatusMessage extends StatusMessage implements Parcelable {

    /**
     * The Friend state of the node.
     */
    public byte friend;

    /**
     * Default constructor for the FriendStatusMessage class.
     */
    public FriendStatusMessage() {
    }

    /**
     * Constructor for the FriendStatusMessage class.
     *
     * @param in The Parcel object containing the message data.
     */
    protected FriendStatusMessage(Parcel in) {
        friend = in.readByte();
    }

    /**
     * Creator constant for the FriendStatusMessage class.
     */
    public static final Creator<FriendStatusMessage> CREATOR = new Creator<FriendStatusMessage>() {
        @Override
        public FriendStatusMessage createFromParcel(Parcel in) {
            return new FriendStatusMessage(in);
        }

        @Override
        public FriendStatusMessage[] newArray(int size) {
            return new FriendStatusMessage[size];
        }
    };

    /**
     * Parses the message parameters.
     *
     * @param params The byte array containing the message parameters.
     */
    @Override
    public void parse(byte[] params) {
        friend = params[0];
    }

    /**
     * Describes the contents of the message.
     *
     * @return An integer value representing the contents of the message.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the message data to a Parcel object.
     *
     * @param dest  The Parcel object to write the message data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(friend);
    }
}