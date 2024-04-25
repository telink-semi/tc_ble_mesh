/********************************************************************************************************
 * @file FastProvisioningEvent.java
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

import com.telink.ble.mesh.entity.FastProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

/**
 * This class represents a FastProvisioningEvent, which is a specific type of Event that is used to notify about events related to fast provisioning.
 * It extends the Event class and has additional properties and methods specific to fast provisioning events.
 */
public class FastProvisioningEvent extends Event<String> {

    // Constants for different types of fast provisioning events
    public static final String EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET";
    public static final String EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET_FAIL = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET_FAIL";
    public static final String EVENT_TYPE_FAST_PROVISIONING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_SUCCESS";
    public static final String EVENT_TYPE_FAST_PROVISIONING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_FAIL";

    // Properties
    private FastProvisioningDevice fastProvisioningDevice; // The fast provisioning device associated with the event
    private String desc; // A description of the event

    /**
     * Constructor for the FastProvisioningEvent class.
     *
     * @param sender the object that sends the event
     * @param type   the type of the event
     */
    public FastProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    /**
     * Constructor for the FastProvisioningEvent class used for parceling.
     *
     * @param in the Parcel object to read the data from
     */
    protected FastProvisioningEvent(Parcel in) {
        fastProvisioningDevice = in.readParcelable(FastProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

    /**
     * Creator constant for the Parcelable interface.
     */
    public static final Creator<FastProvisioningEvent> CREATOR = new Creator<FastProvisioningEvent>() {
        @Override
        public FastProvisioningEvent createFromParcel(Parcel in) {
            return new FastProvisioningEvent(in);
        }

        @Override
        public FastProvisioningEvent[] newArray(int size) {
            return new FastProvisioningEvent[size];
        }
    };

    /**
     * Getter for the description of the event.
     *
     * @return the description of the event
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Setter for the description of the event.
     *
     * @param desc the description of the event
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }

    /**
     * Getter for the fast provisioning device associated with the event.
     *
     * @return the fast provisioning device associated with the event
     */
    public FastProvisioningDevice getFastProvisioningDevice() {
        return fastProvisioningDevice;
    }

    /**
     * Setter for the fast provisioning device associated with the event.
     *
     * @param fastProvisioningDevice the fast provisioning device associated with the event
     */
    public void setFastProvisioningDevice(FastProvisioningDevice fastProvisioningDevice) {
        this.fastProvisioningDevice = fastProvisioningDevice;
    }

    /**
     * Implementation of the describeContents method of the Parcelable interface.
     *
     * @return an integer value representing the contents of the Parcelable object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Implementation of the writeToParcel method of the Parcelable interface.
     *
     * @param dest  the Parcel object to write the data to
     * @param flags additional flags for the Parcelable object
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(fastProvisioningDevice, flags);
        dest.writeString(desc);
    }
}