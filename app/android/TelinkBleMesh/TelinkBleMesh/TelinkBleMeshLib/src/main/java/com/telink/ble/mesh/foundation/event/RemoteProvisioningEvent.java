/********************************************************************************************************
 * @file RemoteProvisioningEvent.java
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

import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

public class RemoteProvisioningEvent extends Event<String> {

    public static final String EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS";

    public static final String EVENT_TYPE_REMOTE_PROVISIONING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_FAIL";

    private RemoteProvisioningDevice remoteProvisioningDevice;
    private String desc;

    public RemoteProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    protected RemoteProvisioningEvent(Parcel in) {
        remoteProvisioningDevice = in.readParcelable(RemoteProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

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

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(remoteProvisioningDevice, flags);
        dest.writeString(desc);
    }

    public RemoteProvisioningDevice getRemoteProvisioningDevice() {
        return remoteProvisioningDevice;
    }

    public void setRemoteProvisioningDevice(RemoteProvisioningDevice remoteProvisioningDevice) {
        this.remoteProvisioningDevice = remoteProvisioningDevice;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }
}
