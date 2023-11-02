/********************************************************************************************************
 * @file DirectedControlStatusMessage.java
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

public class DirectedControlStatusMessage extends StatusMessage implements Parcelable {


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
     * Current Directed Forwarding state
     * 8 bits
     */
    public byte directedForwarding;

    /**
     * Current Directed Relay state
     * 8 bits
     */
    public byte directedRelay;

    /**
     * Current Directed Proxy state or 0xFF
     * 8 bits
     */
    public byte directedProxy;

    /**
     * Current Directed Proxy Use Directed Default state or 0xFF
     * 8 bits
     */
    public byte directedProxyUseDirectedDefault;

    /**
     * Current Directed Friend state or 0xFF
     * 8 bits
     */
    public byte directedFriend;

    /**
     * Default constructor for the DirectedControlStatusMessage class.
     */
    public DirectedControlStatusMessage() {
    }

    /**
     * Constructor for the DirectedControlStatusMessage class that initializes the instance variables from a Parcel.
     *
     * @param in The Parcel containing the params.
     */
    protected DirectedControlStatusMessage(Parcel in) {
        status = in.readInt();
        netKeyIndex = in.readInt();
        directedForwarding = in.readByte();
        directedRelay = in.readByte();
        directedProxy = in.readByte();
        directedProxyUseDirectedDefault = in.readByte();
        directedFriend = in.readByte();
    }

    /**
     * Creator constant for the DirectedControlStatusMessage class.
     */
    public static final Creator<DirectedControlStatusMessage> CREATOR = new Creator<DirectedControlStatusMessage>() {
        @Override
        public DirectedControlStatusMessage createFromParcel(Parcel in) {
            return new DirectedControlStatusMessage(in);
        }

        @Override
        public DirectedControlStatusMessage[] newArray(int size) {
            return new DirectedControlStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to extract the params.
     *
     * @param params The byte array containing the params.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;

        this.status = params[index++];

        this.netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        directedForwarding = params[index++];
        directedRelay = params[index++];
        directedProxy = params[index++];
        directedProxyUseDirectedDefault = params[index++];
        directedFriend = params[index];
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
        dest.writeByte(directedForwarding);
        dest.writeByte(directedRelay);
        dest.writeByte(directedProxy);
        dest.writeByte(directedProxyUseDirectedDefault);
        dest.writeByte(directedFriend);
    }

    /**
     * Returns a string representation of the DirectedControlStatusMessage object.
     *
     * @return A string representation of the DirectedControlStatusMessage object.
     */
    @Override
    public String toString() {
        return "DirectedControlStatusMessage{" +
                "status=" + status +
                ", netKeyIndex=" + netKeyIndex +
                ", directedForwarding=" + directedForwarding +
                ", directedRelay=" + directedRelay +
                ", directedProxy=" + directedProxy +
                ", directedProxyUseDirectedDefault=" + directedProxyUseDirectedDefault +
                ", directedFriend=" + directedFriend +
                '}';
    }
}
