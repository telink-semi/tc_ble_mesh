/********************************************************************************************************
 * @file ProvisioningEvent.java
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

import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/4.
 */

public class ProvisioningEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_PROVISION_GET_REVISION = "com.telink.ble.mesh.EVENT_TYPE_PROVISION_GET_REVISION";

    public static final String EVENT_TYPE_PROVISION_BEGIN = "com.telink.ble.mesh.EVENT_TYPE_PROVISION_BEGIN";

    public static final String EVENT_TYPE_PROVISION_SUCCESS = "com.telink.ble.mesh.EVENT_TYPE_PROVISION_SUCCESS";

    public static final String EVENT_TYPE_PROVISION_FAIL = "com.telink.ble.mesh.EVENT_TYPE_PROVISION_FAIL";

    private ProvisioningDevice provisioningDevice;
    private String desc;

    public ProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    public ProvisioningEvent(Object sender, String type, ProvisioningDevice provisioningDevice) {
        super(sender, type);
        this.provisioningDevice = provisioningDevice;
    }

    public ProvisioningEvent(Object sender, String type, ProvisioningDevice provisioningDevice, String desc) {
        super(sender, type);
        this.provisioningDevice = provisioningDevice;
        this.desc = desc;
    }

    protected ProvisioningEvent(Parcel in) {
        provisioningDevice = in.readParcelable(ProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

    public static final Creator<ProvisioningEvent> CREATOR = new Creator<ProvisioningEvent>() {
        @Override
        public ProvisioningEvent createFromParcel(Parcel in) {
            return new ProvisioningEvent(in);
        }

        @Override
        public ProvisioningEvent[] newArray(int size) {
            return new ProvisioningEvent[size];
        }
    };

    public ProvisioningDevice getProvisioningDevice() {
        return provisioningDevice;
    }

    public String getDesc() {
        return desc;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(provisioningDevice, flags);
        dest.writeString(desc);
    }
}
