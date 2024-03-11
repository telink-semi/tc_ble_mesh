/********************************************************************************************************
 * @file BindingEvent.java
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

import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * This class represents a binding event that can occur during the process of binding a device.
 * It extends the Event class and contains information about the binding device and a description of the event.
 */
public class BindingEvent extends Event<String> {

    public static final String EVENT_TYPE_BIND_SUCCESS = "com.telink.ble.mesh.EVENT_TYPE_BIND_SUCCESS";
    public static final String EVENT_TYPE_BIND_FAIL = "com.telink.ble.mesh.EVENT_TYPE_BIND_FAIL";

    private BindingDevice bindingDevice; // The device being bound
    private String desc; // Description of the event

    /**
     * Constructor for the BindingEvent class.
     *
     * @param sender        The object that sent the event
     * @param type          The type of the event
     * @param bindingDevice The device being bound
     * @param desc          The description of the event
     */
    public BindingEvent(Object sender, String type, BindingDevice bindingDevice, String desc) {
        super(sender, type);
        this.bindingDevice = bindingDevice;
        this.desc = desc;
    }

    /**
     * Constructor for the BindingEvent class used for parceling.
     *
     * @param in The parcel to read from
     */
    protected BindingEvent(Parcel in) {
        bindingDevice = in.readParcelable(BindingDevice.class.getClassLoader());
        desc = in.readString();
    }

    /**
     * Creator for the BindingEvent class used for parceling.
     */
    public static final Creator<BindingEvent> CREATOR = new Creator<BindingEvent>() {
        @Override
        public BindingEvent createFromParcel(Parcel in) {
            return new BindingEvent(in);
        }

        @Override
        public BindingEvent[] newArray(int size) {
            return new BindingEvent[size];
        }
    };

    /**
     * Getter for the description of the event.
     *
     * @return The description of the event
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Setter for the description of the event.
     *
     * @param desc The description of the event
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }

    /**
     * Getter for the device being bound.
     *
     * @return The device being bound
     */
    public BindingDevice getBindingDevice() {
        return bindingDevice;
    }

    /**
     * Constructor for the BindingEvent class.
     *
     * @param sender The object that sent the event
     * @param type   The type of the event
     */
    public BindingEvent(Object sender, String type) {
        super(sender, type);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(bindingDevice, flags);
        dest.writeString(desc);
    }
}