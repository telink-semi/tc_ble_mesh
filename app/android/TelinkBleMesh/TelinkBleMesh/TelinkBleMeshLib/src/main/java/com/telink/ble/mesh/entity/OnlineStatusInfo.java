/********************************************************************************************************
 * @file OnlineStatusInfo.java
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

package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * online status
 * This class represents the online status information of a user.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 */
public class OnlineStatusInfo implements Parcelable {

    public int address; // The address of the user
    public byte sn; // The serial number indicating the online status (0 for offline)
    public byte[] status; // The additional status information

    /**
     * Default constructor for the OnlineStatusInfo class.
     */
    public OnlineStatusInfo() {
    }

    /**
     * Constructor for the OnlineStatusInfo class that takes a Parcel as input.
     * Used for deserialization.
     *
     * @param in The Parcel containing the serialized OnlineStatusInfo object.
     */
    protected OnlineStatusInfo(Parcel in) {
        address = in.readInt();
        sn = in.readByte();
        status = in.createByteArray();
    }

    /**
     * Creator constant for the Parcelable interface.
     */
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

    /**
     * Returns a bitmask indicating the set of special object types
     * describing the OnlineStatusInfo instance.
     *
     * @return A bitmask indicating the set of special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the OnlineStatusInfo object's data to the given Parcel.
     *
     * @param dest  The Parcel to write the object's data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(address);
        dest.writeByte(sn);
        dest.writeByteArray(status);
    }
}