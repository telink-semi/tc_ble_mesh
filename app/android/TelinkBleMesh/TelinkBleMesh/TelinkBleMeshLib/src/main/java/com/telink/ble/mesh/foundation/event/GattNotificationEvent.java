/********************************************************************************************************
 * @file GattNotificationEvent.java
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
package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.foundation.Event;

import java.util.UUID;

/**
 * Created by kee on 2019/9/12.
 */

public class GattNotificationEvent extends Event<String> implements Parcelable {
    public static final String EVENT_TYPE_UNEXPECTED_NOTIFY = "com.telink.ble.mesh.EVENT_TYPE_UNEXPECTED_NOTIFY";

    private UUID serviceUUID;
    private UUID characteristicUUID;
    private byte[] data;


    public GattNotificationEvent(Object sender, String type, UUID serviceUUID, UUID characteristicUUID, byte[] data) {
        super(sender, type);
        this.serviceUUID = serviceUUID;
        this.characteristicUUID = characteristicUUID;
        this.data = data;
    }

    protected GattNotificationEvent(Parcel in) {
        serviceUUID = UUID.fromString(in.readString());
        characteristicUUID = UUID.fromString(in.readString());
        data = in.createByteArray();
    }


    public UUID getServiceUUID() {
        return serviceUUID;
    }

    public UUID getCharacteristicUUID() {
        return characteristicUUID;
    }

    public byte[] getData() {
        return data;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(serviceUUID.toString());
        dest.writeString(characteristicUUID.toString());
        dest.writeByteArray(data);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<GattNotificationEvent> CREATOR = new Creator<GattNotificationEvent>() {
        @Override
        public GattNotificationEvent createFromParcel(Parcel in) {
            return new GattNotificationEvent(in);
        }

        @Override
        public GattNotificationEvent[] newArray(int size) {
            return new GattNotificationEvent[size];
        }
    };
}
