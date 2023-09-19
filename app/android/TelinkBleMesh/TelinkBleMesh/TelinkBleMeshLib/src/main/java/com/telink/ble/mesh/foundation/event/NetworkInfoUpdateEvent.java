/********************************************************************************************************
 * @file NetworkInfoUpdateEvent.java
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
package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/4.
 */

public class NetworkInfoUpdateEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_NETWORKD_INFO_UPDATE = "com.telink.ble.mesh.EVENT_TYPE_NETWORKD_INFO_UPDATE";

    private int sequenceNumber;

    private int ivIndex;

    public NetworkInfoUpdateEvent(Object sender, String type, int sequenceNumber, int ivIndex) {
        super(sender, type);
        this.sequenceNumber = sequenceNumber;
        this.ivIndex = ivIndex;
    }

    protected NetworkInfoUpdateEvent(Parcel in) {
        sequenceNumber = in.readInt();
        ivIndex = in.readInt();
    }

    public static final Creator<NetworkInfoUpdateEvent> CREATOR = new Creator<NetworkInfoUpdateEvent>() {
        @Override
        public NetworkInfoUpdateEvent createFromParcel(Parcel in) {
            return new NetworkInfoUpdateEvent(in);
        }

        @Override
        public NetworkInfoUpdateEvent[] newArray(int size) {
            return new NetworkInfoUpdateEvent[size];
        }
    };

    public int getSequenceNumber() {
        return sequenceNumber;
    }

    public int getIvIndex() {
        return ivIndex;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(sequenceNumber);
        dest.writeInt(ivIndex);
    }
}
