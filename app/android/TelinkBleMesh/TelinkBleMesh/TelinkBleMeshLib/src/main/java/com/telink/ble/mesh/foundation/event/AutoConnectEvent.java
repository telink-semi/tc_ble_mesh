/********************************************************************************************************
 * @file AutoConnectEvent.java
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

import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/12.
 */

public class AutoConnectEvent extends Event<String> {
    public static final String EVENT_TYPE_AUTO_CONNECT_LOGIN = "com.telink.ble.mesh.EVENT_TYPE_AUTO_CONNECT_LOGIN";

//    public static final String EVENT_TYPE_AUTO_CONNECT_LOGOUT = "com.telink.ble.mesh.EVENT_TYPE_AUTO_CONNECT_LOGOUT";

    private int connectedAddress;

    public AutoConnectEvent(Object sender, String type, int connectedAddress) {
        super(sender, type);
        this.connectedAddress = connectedAddress;
    }

    protected AutoConnectEvent(Parcel in) {
        connectedAddress = in.readInt();
    }

    public static final Creator<AutoConnectEvent> CREATOR = new Creator<AutoConnectEvent>() {
        @Override
        public AutoConnectEvent createFromParcel(Parcel in) {
            return new AutoConnectEvent(in);
        }

        @Override
        public AutoConnectEvent[] newArray(int size) {
            return new AutoConnectEvent[size];
        }
    };

    public int getConnectedAddress() {
        return connectedAddress;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(connectedAddress);
    }
}
