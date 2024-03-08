/********************************************************************************************************
 * @file RemoteProvisioningEvent.java
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

import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * The RemoteProvisioningEvent class represents an event related to remote provisioning.
 * It extends the Event class and contains specific properties and methods related to remote provisioning events.
 */
public class RemoteProvisioningEvent extends Event<String> {

    /**
     * Constant for the event type of a successful remote provisioning.
     */
    public static final String EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS";

    /**
     * Constant for the event type of a failed remote provisioning.
     */
    public static final String EVENT_TYPE_REMOTE_PROVISIONING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_FAIL";

    public static final String EVENT_TYPE_REMOTE_CAPABILITY_RECEIVED = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_CAPABILITY_RECEIVED";

    private RemoteProvisioningDevice remoteProvisioningDevice;
    private String desc;

    /**
     * Constructor for the RemoteProvisioningEvent class.
     *
     * @param sender the object that is sending the event
     * @param type   the type of the event
     */
    public RemoteProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    /**
     * Constructor for the RemoteProvisioningEvent class used for parceling.
     *
     * @param in the parcel to read from
     */
    protected RemoteProvisioningEvent(Parcel in) {
        remoteProvisioningDevice = in.readParcelable(RemoteProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

    /**
     * Creator for the RemoteProvisioningEvent class used for parceling.
     */
    public static final Creator<RemoteProvisioningEvent> CREATOR = new Creator<RemoteProvisioningEvent>() {
        @Override
        public RemoteProvisioningEvent createFromParcel(Parcel in) {
            return new RemoteProvisioningEvent(in);
        }

        @Override
        public RemoteProvisioningEvent[] newArray(int size) {
            return new RemoteProvisioningEvent[size];
        }
    };

    /**
     * {@inheritDoc}
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(remoteProvisioningDevice, flags);
        dest.writeString(desc);
    }

    /**
     * Returns the remote provisioning device associated with the event.
     *
     * @return the remote provisioning device
     */
    public RemoteProvisioningDevice getRemoteProvisioningDevice() {
        return remoteProvisioningDevice;
    }

    /**
     * Sets the remote provisioning device associated with the event.
     *
     * @param remoteProvisioningDevice the remote provisioning device to set
     */
    public void setRemoteProvisioningDevice(RemoteProvisioningDevice remoteProvisioningDevice) {
        this.remoteProvisioningDevice = remoteProvisioningDevice;
    }

    /**
     * Returns the description of the event.
     *
     * @return the event description
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Sets the description of the event.
     *
     * @param desc the event description to set
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }
}