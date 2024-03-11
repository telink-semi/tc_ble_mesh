/********************************************************************************************************
 * @file ForwardingTableStatusMessage.java
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
package com.telink.ble.mesh.core.message.directforwarding;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a Forwarding Table Status Message in a network.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer between components.
 */
public class ForwardingTableStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Status code for the requesting message
     * 8 bits
     */
    public int status;

    /**
     * NetKey Index of the NetKey used in the subnet
     * 16 bits
     */
    public int netKeyIndex;

    /**
     * Primary element address of the Path Origin
     * 16 bits
     */
    public int pathOrigin;

    /**
     * Destination address
     * 16 bits
     */
    public int destination;


    /**
     * Default constructor for the ForwardingTableStatusMessage class.
     */
    public ForwardingTableStatusMessage() {
    }

    /**
     * Constructor for the ForwardingTableStatusMessage class that initializes the instance variables from a Parcel.
     *
     * @param in The Parcel containing the status code and receivers list count.
     */
    protected ForwardingTableStatusMessage(Parcel in) {
        status = in.readInt();
        netKeyIndex = in.readInt();
        pathOrigin = in.readInt();
        destination = in.readInt();
    }

    /**
     * Creator constant for the ForwardingTableStatusMessage class.
     */
    public static final Creator<ForwardingTableStatusMessage> CREATOR = new Creator<ForwardingTableStatusMessage>() {
        @Override
        public ForwardingTableStatusMessage createFromParcel(Parcel in) {
            return new ForwardingTableStatusMessage(in);
        }

        @Override
        public ForwardingTableStatusMessage[] newArray(int size) {
            return new ForwardingTableStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to params.
     *
     * @param params The byte array containing the params.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;

        this.status = params[index++];

        this.netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.pathOrigin = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.destination = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Implements the describeContents() method from the Parcelable interface.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the params to the Parcel.
     *
     * @param dest  The Parcel to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(netKeyIndex);
        dest.writeInt(pathOrigin);
        dest.writeInt(destination);
    }

    /**
     * Returns a string representation of the ForwardingTableStatusMessage object.
     *
     * @return A string representation of the ForwardingTableStatusMessage object.
     */
    @Override
    public String toString() {
        return "ForwardingTableStatusMessage{" +
                "status=" + status +
                ", netKeyIndex=" + netKeyIndex +
                ", pathOrigin=" + pathOrigin +
                ", destination=" + destination +
                '}';
    }
}
