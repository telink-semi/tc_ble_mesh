/********************************************************************************************************
 * @file BridgingTableStatusMessage.java
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
 * Created by kee on 2021/1/14.
 */
public class BridgingTableStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Status Code for the requesting message
     * 8 bits
     */
    private int status;

    /**
     * Allowed directions for bridged traffic or bridged traffic not allowed
     * 8 bits
     */
    private byte currentDirections;

    /**
     * NetKey Index of the first subnet
     * 12 bits
     */
    public int netKeyIndex1;

    /**
     * NetKey Index of the second subnet
     * 12 bits
     */
    public int netKeyIndex2;

    /**
     * Address of the node in the first subnet
     * 16 bits
     */
    public int address1;

    /**
     * Address of the node in the second subnet
     * 16 bits
     */
    public int address2;


    public BridgingTableStatusMessage() {
    }


    protected BridgingTableStatusMessage(Parcel in) {
        status = in.readInt();
        currentDirections = in.readByte();
        netKeyIndex1 = in.readInt();
        netKeyIndex2 = in.readInt();
        address1 = in.readInt();
        address2 = in.readInt();
    }

    public static final Creator<BridgingTableStatusMessage> CREATOR = new Creator<BridgingTableStatusMessage>() {
        @Override
        public BridgingTableStatusMessage createFromParcel(Parcel in) {
            return new BridgingTableStatusMessage(in);
        }

        @Override
        public BridgingTableStatusMessage[] newArray(int size) {
            return new BridgingTableStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++] & 0xFF;
        this.currentDirections = params[index++];

        int netKeyIndexes = MeshUtils.bytes2Integer(new byte[]{
                params[index++], params[index++], params[index++],
        }, ByteOrder.LITTLE_ENDIAN);

        this.netKeyIndex1 = netKeyIndexes & 0x0FFF;
        this.netKeyIndex2 = (netKeyIndexes >> 12) & 0x0FFF;


        this.address1 = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.address2 = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }


    public int getStatus() {
        return status;
    }

    public byte getCurrentDirections() {
        return currentDirections;
    }

    public int getNetKeyIndex1() {
        return netKeyIndex1;
    }

    public int getNetKeyIndex2() {
        return netKeyIndex2;
    }

    public int getAddress1() {
        return address1;
    }

    public int getAddress2() {
        return address2;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeByte(currentDirections);
        dest.writeInt(netKeyIndex1);
        dest.writeInt(netKeyIndex2);
        dest.writeInt(address1);
        dest.writeInt(address2);
    }
}
