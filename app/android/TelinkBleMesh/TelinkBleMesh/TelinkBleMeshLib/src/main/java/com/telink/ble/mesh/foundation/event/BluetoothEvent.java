/********************************************************************************************************
 * @file BluetoothEvent.java
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

/**
 * Created by kee on 2019/9/4.
 */

/**
 * Represents a Bluetooth event.
 * Extends the Event class and implements the Parcelable interface.
 */
public class BluetoothEvent extends Event<String> implements Parcelable {

    /**
     * The event type for a Bluetooth state change.
     */
    public static final String EVENT_TYPE_BLUETOOTH_STATE_CHANGE = "com.telink.ble.mesh.EVENT_TYPE_BLUETOOTH_STATE_CHANGE";

    private int state;
    private String desc;

    /**
     * Constructs a new BluetoothEvent object.
     *
     * @param sender the object that sends the event
     * @param type   the type of the event
     */
    public BluetoothEvent(Object sender, String type) {
        super(sender, type);
    }

    /**
     * Gets the state of the Bluetooth.
     *
     * @return the state of the Bluetooth
     */
    public int getState() {
        return state;
    }

    /**
     * Sets the state of the Bluetooth.
     *
     * @param state the state of the Bluetooth
     */
    public void setState(int state) {
        this.state = state;
    }

    /**
     * Gets the description of the Bluetooth event.
     *
     * @return the description of the Bluetooth event
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Sets the description of the Bluetooth event.
     *
     * @param desc the description of the Bluetooth event
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }

    /**
     * Constructs a BluetoothEvent object from a Parcel.
     *
     * @param in the Parcel object to read the data from
     */
    protected BluetoothEvent(Parcel in) {
        state = in.readInt();
        desc = in.readString();
    }

    /**
     * Creates a new BluetoothEvent object from a Parcel.
     */
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

    /**
     * Describes the contents of the BluetoothEvent object.
     *
     * @return an integer representing the contents of the object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the data of the BluetoothEvent object to a Parcel.
     *
     * @param dest  the Parcel object to write the data to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(state);
        dest.writeString(desc);
    }
}