/********************************************************************************************************
 * @file BluetoothEvent.java
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

public class BluetoothEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_BLUETOOTH_STATE_CHANGE = "com.telink.ble.mesh.EVENT_TYPE_BLUETOOTH_STATE_CHANGE";

    private int state;

    private String desc;

    public BluetoothEvent(Object sender, String type) {
        super(sender, type);
    }

    public int getState() {
        return state;
    }

    public void setState(int state) {
        this.state = state;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    protected BluetoothEvent(Parcel in) {
        state = in.readInt();
        desc = in.readString();
    }

    public static final Creator<BluetoothEvent> CREATOR = new Creator<BluetoothEvent>() {
        @Override
        public BluetoothEvent createFromParcel(Parcel in) {
            return new BluetoothEvent(in);
        }

        @Override
        public BluetoothEvent[] newArray(int size) {
            return new BluetoothEvent[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(state);
        dest.writeString(desc);
    }
}
