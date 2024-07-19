/********************************************************************************************************
 * @file MeshInfoUpdateEvent.java
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
package com.telink.ble.mesh.web;

import android.os.Parcel;

import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/12.
 */

public class MeshInfoUpdateEvent extends Event<String> {
    public static final String EVENT_TYPE_NODES_UPDATE = "com.telink.ble.mesh.EVENT_TYPE_NODES_UPDATE";

    public static final String EVENT_TYPE_GROUPS_UPDATE = "com.telink.ble.mesh.EVENT_TYPE_GROUPS_UPDATE";

    public MeshInfoUpdateEvent(Object sender, String type) {
        super(sender, type);
    }

    protected MeshInfoUpdateEvent(Parcel in) {

    }

    public static final Creator<MeshInfoUpdateEvent> CREATOR = new Creator<MeshInfoUpdateEvent>() {
        @Override
        public MeshInfoUpdateEvent createFromParcel(Parcel in) {
            return new MeshInfoUpdateEvent(in);
        }

        @Override
        public MeshInfoUpdateEvent[] newArray(int size) {
            return new MeshInfoUpdateEvent[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
    }
}
