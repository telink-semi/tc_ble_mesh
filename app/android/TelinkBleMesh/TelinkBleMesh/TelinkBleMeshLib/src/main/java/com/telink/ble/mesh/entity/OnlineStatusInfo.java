/********************************************************************************************************
 * @file OnlineStatusInfo.java
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

package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * online status
 */
public class OnlineStatusInfo implements Parcelable {

    public int address;

//        byte rsv; // 1 bit

    // sn: 0 offline
    public byte sn;

    public byte[] status;

    public OnlineStatusInfo() {
    }

    protected OnlineStatusInfo(Parcel in) {
        address = in.readInt();
        sn = in.readByte();
        status = in.createByteArray();
    }

    public static final Creator<OnlineStatusInfo> CREATOR = new Creator<OnlineStatusInfo>() {
        @Override
        public OnlineStatusInfo createFromParcel(Parcel in) {
            return new OnlineStatusInfo(in);
        }

        @Override
        public OnlineStatusInfo[] newArray(int size) {
            return new OnlineStatusInfo[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(address);
        dest.writeByte(sn);
        dest.writeByteArray(status);
    }
}
