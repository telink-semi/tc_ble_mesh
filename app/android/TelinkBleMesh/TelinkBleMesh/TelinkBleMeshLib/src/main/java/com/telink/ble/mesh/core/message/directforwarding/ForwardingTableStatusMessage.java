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


    public ForwardingTableStatusMessage() {
    }

    protected ForwardingTableStatusMessage(Parcel in) {
        status = in.readInt();
        netKeyIndex = in.readInt();
        pathOrigin = in.readInt();
        destination = in.readInt();
    }

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

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(netKeyIndex);
        dest.writeInt(pathOrigin);
        dest.writeInt(destination);
    }

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
