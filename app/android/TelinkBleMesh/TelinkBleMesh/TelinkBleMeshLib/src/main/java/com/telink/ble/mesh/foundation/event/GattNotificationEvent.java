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

/**
 * Represents a GATT notification event.
 */
public class GattNotificationEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_UNEXPECTED_NOTIFY = "com.telink.ble.mesh.EVENT_TYPE_UNEXPECTED_NOTIFY";

    private UUID serviceUUID;
    private UUID characteristicUUID;
    private byte[] data;

    /**
     * Constructs a GattNotificationEvent object.
     *
     * @param sender             the object that sends the event
     * @param type               the type of the event
     * @param serviceUUID        the UUID of the service
     * @param characteristicUUID the UUID of the characteristic
     * @param data               the data received in the notification
     */
    public GattNotificationEvent(Object sender, String type, UUID serviceUUID, UUID characteristicUUID, byte[] data) {
        super(sender, type);
        this.serviceUUID = serviceUUID;
        this.characteristicUUID = characteristicUUID;
        this.data = data;
    }

    /**
     * Constructs a GattNotificationEvent object from a Parcel.
     *
     * @param in the Parcel object to read the data from
     */
    protected GattNotificationEvent(Parcel in) {
        serviceUUID = UUID.fromString(in.readString());
        characteristicUUID = UUID.fromString(in.readString());
        data = in.createByteArray();
    }

    /**
     * Gets the UUID of the service.
     *
     * @return the UUID of the service
     */
    public UUID getServiceUUID() {
        return serviceUUID;
    }

    /**
     * Gets the UUID of the characteristic.
     *
     * @return the UUID of the characteristic
     */
    public UUID getCharacteristicUUID() {
        return characteristicUUID;
    }

    /**
     * Gets the data received in the notification.
     *
     * @return the data received in the notification
     */
    public byte[] getData() {
        return data;
    }

    /**
     * Writes the object's data to a Parcel.
     *
     * @param dest  the Parcel object to write the data to
     * @param flags additional flags for the object
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(serviceUUID.toString());
        dest.writeString(characteristicUUID.toString());
        dest.writeByteArray(data);
    }

    /**
     * Describes the contents of the object.
     *
     * @return an integer representing the contents of the object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Generates instances of the Parcelable class from a Parcel.
     */
    public static final Creator<GattNotificationEvent> CREATOR = new Creator<GattNotificationEvent>() {
        /**
         * Creates a new GattNotificationEvent object from a Parcel.
         *
         * @param in the Parcel object to read the data from
         * @return the created GattNotificationEvent object
         */
        @Override
        public GattNotificationEvent createFromParcel(Parcel in) {
            return new GattNotificationEvent(in);
        }

        /**
         * Creates a new array of the Parcelable class.
         *
         * @param size the size of the array
         * @return an array of the Parcelable class
         */
        @Override
        public GattNotificationEvent[] newArray(int size) {
            return new GattNotificationEvent[size];
        }
    };
}