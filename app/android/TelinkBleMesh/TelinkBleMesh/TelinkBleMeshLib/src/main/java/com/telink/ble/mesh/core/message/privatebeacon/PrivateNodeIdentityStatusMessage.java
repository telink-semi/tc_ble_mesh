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
 */

public class PrivateNodeIdentityStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * Status Code for the requesting message
     */
    public byte status;

    /**
     * 2 bytes
     * Index of the NetKey
     */
    public int netKeyIndex;

    /**
     * 1 byte
     * Private Node Identity state
     */
    public byte privateIdentity;

    public PrivateNodeIdentityStatusMessage() {
    }


    protected PrivateNodeIdentityStatusMessage(Parcel in) {
        status = in.readByte();
        netKeyIndex = in.readInt();
        privateIdentity = in.readByte();
    }

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

    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++];
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        privateIdentity = params[index];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(netKeyIndex);
        dest.writeByte(privateIdentity);
    }
}
