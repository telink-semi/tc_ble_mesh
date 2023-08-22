/********************************************************************************************************
 * @file AppKeyStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config AppKey Status is an unacknowledged message used to report a status for the requesting message,
 * based on the NetKey Index identifying the NetKey on the NetKey List and on the AppKey Index identifying the AppKey on the AppKey List.
 */
public class AppKeyStatusMessage extends StatusMessage {

    /**
     * 1 byte
     * Status Code for the requesting message
     */
    private byte status;

    /**
     * 12 bits
     */
    private int netKeyIndex;

    /**
     * 12 bits
     */
    private int appKeyIndex;


    public AppKeyStatusMessage() {
    }

    protected AppKeyStatusMessage(Parcel in) {
        status = in.readByte();
        netKeyIndex = in.readInt();
        appKeyIndex = in.readInt();
    }

    public static final Creator<AppKeyStatusMessage> CREATOR = new Creator<AppKeyStatusMessage>() {
        @Override
        public AppKeyStatusMessage createFromParcel(Parcel in) {
            return new AppKeyStatusMessage(in);
        }

        @Override
        public AppKeyStatusMessage[] newArray(int size) {
            return new AppKeyStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        status = params[0];

        int netAppKeyIndex = MeshUtils.bytes2Integer(new byte[]{
                params[1], params[2], params[3],
        }, ByteOrder.LITTLE_ENDIAN);

        this.netKeyIndex = netAppKeyIndex & 0x0FFF;
        this.appKeyIndex = (netAppKeyIndex >> 12) & 0x0FFF;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(netKeyIndex);
        dest.writeInt(appKeyIndex);
    }

    public byte getStatus() {
        return status;
    }

    public int getNetKeyIndex() {
        return netKeyIndex;
    }

    public int getAppKeyIndex() {
        return appKeyIndex;
    }

}

