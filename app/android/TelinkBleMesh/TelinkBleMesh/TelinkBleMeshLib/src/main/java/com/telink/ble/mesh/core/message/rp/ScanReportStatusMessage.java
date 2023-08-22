/********************************************************************************************************
 * @file ScanReportStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class ScanReportStatusMessage extends StatusMessage implements Parcelable {

    private byte rssi;

    // 16 bytes
    private byte[] uuid;

    // 2 bytes
    private int oob;

    public ScanReportStatusMessage() {
    }

    protected ScanReportStatusMessage(Parcel in) {
        rssi = in.readByte();
        uuid = in.createByteArray();
        oob = in.readInt();
    }

    public static final Creator<ScanReportStatusMessage> CREATOR = new Creator<ScanReportStatusMessage>() {
        @Override
        public ScanReportStatusMessage createFromParcel(Parcel in) {
            return new ScanReportStatusMessage(in);
        }

        @Override
        public ScanReportStatusMessage[] newArray(int size) {
            return new ScanReportStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.rssi = params[index++];
        this.uuid = new byte[16];
        System.arraycopy(params, index, this.uuid, 0, this.uuid.length);
        index += this.uuid.length;
        this.oob = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(rssi);
        dest.writeByteArray(uuid);
        dest.writeInt(oob);
    }

    public byte getRssi() {
        return rssi;
    }

    public byte[] getUuid() {
        return uuid;
    }

    public int getOob() {
        return oob;
    }
}
