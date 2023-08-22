/********************************************************************************************************
 * @file FirmwareUpdatingEvent.java
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

import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * @see com.telink.ble.mesh.core.access.fu.FUCallback
 * Created by kee on 2017/8/30.
 * @deprecated
 */

public class FirmwareUpdatingEvent extends Event<String> {

    public static final String EVENT_TYPE_UPDATING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_SUCCESS";

    public static final String EVENT_TYPE_UPDATING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_FAIL";

    public static final String EVENT_TYPE_UPDATING_PROGRESS = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_PROGRESS";

    public static final String EVENT_TYPE_UPDATING_STOPPED = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_STOPPED";

    public static final String EVENT_TYPE_DEVICE_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_SUCCESS";

    public static final String EVENT_TYPE_DEVICE_FAIL = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_FAIL";

    public static final String EVENT_TYPE_UPDATING_PREPARED = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_PREPARED";

    private MeshUpdatingDevice updatingDevice;
    private int progress;
    private String desc;

    public FirmwareUpdatingEvent(Object sender, String type) {
        super(sender, type);
    }

    protected FirmwareUpdatingEvent(Parcel in) {
        updatingDevice = in.readParcelable(MeshUpdatingDevice.class.getClassLoader());
        progress = in.readInt();
        desc = in.readString();
    }

    public static final Creator<FirmwareUpdatingEvent> CREATOR = new Creator<FirmwareUpdatingEvent>() {
        @Override
        public FirmwareUpdatingEvent createFromParcel(Parcel in) {
            return new FirmwareUpdatingEvent(in);
        }

        @Override
        public FirmwareUpdatingEvent[] newArray(int size) {
            return new FirmwareUpdatingEvent[size];
        }
    };

    public void setUpdatingDevice(MeshUpdatingDevice updatingDevice) {
        this.updatingDevice = updatingDevice;
    }

    public void setProgress(int progress) {
        this.progress = progress;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    public MeshUpdatingDevice getUpdatingDevice() {
        return updatingDevice;
    }

    public int getProgress() {
        return progress;
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
        dest.writeParcelable(updatingDevice, flags);
        dest.writeInt(progress);
        dest.writeString(desc);
    }
}
