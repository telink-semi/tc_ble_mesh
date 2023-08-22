/********************************************************************************************************
 * @file ScanEvent.java
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
import android.os.Parcelable;

import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/5.
 */

public class ScanEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_DEVICE_FOUND = "com.telink.ble.mesh.EVENT_TYPE_DEVICE_FOUND";

    public static final String EVENT_TYPE_SCAN_TIMEOUT = "com.telink.ble.mesh.EVENT_TYPE_SCAN_TIMEOUT";

    public static final String EVENT_TYPE_SCAN_FAIL = "com.telink.ble.mesh.EVENT_TYPE_SCAN_FAIL";

    public static final String EVENT_TYPE_SCAN_LOCATION_WARNING = "com.telink.ble.mesh.EVENT_TYPE_SCAN_LOCATION_WARNING";

    private AdvertisingDevice advertisingDevice;

    public ScanEvent(Object sender, String type, AdvertisingDevice advertisingDevice) {
        super(sender, type);
        this.advertisingDevice = advertisingDevice;
    }

    public ScanEvent(Object sender, String type) {
        super(sender, type);
    }

    protected ScanEvent(Parcel in) {
        advertisingDevice = in.readParcelable(AdvertisingDevice.class.getClassLoader());
    }

    public static final Creator<ScanEvent> CREATOR = new Creator<ScanEvent>() {
        @Override
        public ScanEvent createFromParcel(Parcel in) {
            return new ScanEvent(in);
        }

        @Override
        public ScanEvent[] newArray(int size) {
            return new ScanEvent[size];
        }
    };

    public AdvertisingDevice getAdvertisingDevice() {
        return advertisingDevice;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(advertisingDevice, flags);
    }
}
