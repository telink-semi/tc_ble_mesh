/********************************************************************************************************
 * @file PrivateNodeIdentityStatusMessage.java
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
package com.telink.ble.mesh.core.message.privatebeacon;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * A PRIVATE_NODE_IDENTITY_STATUS message is an unacknowledged message used to
 * report the current Private Node Identity state for a subnet
 * The PrivateNodeIdentityStatusMessage class represents an unacknowledged message used to report the current Private Node Identity state for a subnet.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class PrivateNodeIdentityStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Represents the status code for the requesting message.
     * It is a 1-byte value.
     */
    public byte status;

    /**
     * Represents the index of the NetKey.
     * It is a 2-byte value.
     */
    public int netKeyIndex;

    /**
     * Represents the Private Node Identity state.
     * It is a 1-byte value.
     */
    public byte privateIdentity;

    /**
     * Constructs a new PrivateNodeIdentityStatusMessage object.
     */
    public PrivateNodeIdentityStatusMessage() {
    }

    /**
     * Constructs a new PrivateNodeIdentityStatusMessage object from a Parcel.
     *
     * @param in The Parcel object to read the data from.
     */
    protected PrivateNodeIdentityStatusMessage(Parcel in) {
        status = in.readByte();
        netKeyIndex = in.readInt();
        privateIdentity = in.readByte();
    }

    /**
     * A Creator object that is used to create new instances of the PrivateNodeIdentityStatusMessage class.
     */
    public static final Creator<PrivateNodeIdentityStatusMessage> CREATOR = new Creator<PrivateNodeIdentityStatusMessage>() {
        @Override
        public PrivateNodeIdentityStatusMessage createFromParcel(Parcel in) {
            return new PrivateNodeIdentityStatusMessage(in);
        }

        @Override
        public PrivateNodeIdentityStatusMessage[] newArray(int size) {
            return new PrivateNodeIdentityStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array to populate the fields of the PrivateNodeIdentityStatusMessage object.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++];
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        privateIdentity = params[index];
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0 as this Parcelable implementation does not have any special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the fields of the PrivateNodeIdentityStatusMessage object to the given Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(netKeyIndex);
        dest.writeByte(privateIdentity);
    }
}