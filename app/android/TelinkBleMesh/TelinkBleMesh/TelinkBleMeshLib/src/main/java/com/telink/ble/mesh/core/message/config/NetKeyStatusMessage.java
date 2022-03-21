/********************************************************************************************************
 * @file NetKeyStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config NetKey Status is an unacknowledged message used to report the status of the operation on the NetKey List.
 */

public class NetKeyStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * Status Code for the requesting message
     */
    private int status;

    /**
     * 2 bytes
     * Index of the NetKey
     */
    private int netKeyIndex;


    public NetKeyStatusMessage() {
    }


    protected NetKeyStatusMessage(Parcel in) {
        status = in.readInt();
        netKeyIndex = in.readInt();
    }

    public static final Creator<NetKeyStatusMessage> CREATOR = new Creator<NetKeyStatusMessage>() {
        @Override
        public NetKeyStatusMessage createFromParcel(Parcel in) {
            return new NetKeyStatusMessage(in);
        }

        @Override
        public NetKeyStatusMessage[] newArray(int size) {
            return new NetKeyStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++] & 0xFF;
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    public int getStatus() {
        return status;
    }

    public int getNetKeyIndex() {
        return netKeyIndex;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(netKeyIndex);
    }
}
