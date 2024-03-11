/********************************************************************************************************
 * @file GattOtaEvent.java
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

import com.telink.ble.mesh.foundation.Event;

/**
 * This class represents a GattOtaEvent, which is an event related to GATT OTA (Over-the-Air) updates.
 * It extends the Event class and contains additional properties for progress and description.
 */
public class GattOtaEvent extends Event<String> {

    public static final String EVENT_TYPE_OTA_SUCCESS = "com.telink.sig.mesh.OTA_SUCCESS";
    public static final String EVENT_TYPE_OTA_FAIL = "com.telink.sig.mesh.OTA_FAIL";
    public static final String EVENT_TYPE_OTA_PROGRESS = "com.telink.sig.mesh.OTA_PROGRESS";

    private int progress;
    private String desc;

    /**
     * Constructor for GattOtaEvent.
     *
     * @param sender   the object that sends the event
     * @param type     the type of the event
     * @param progress the progress of the OTA update
     * @param desc     a description of the event
     */
    public GattOtaEvent(Object sender, String type, int progress, String desc) {
        super(sender, type);
        this.progress = progress;
        this.desc = desc;
    }

    /**
     * Constructor for GattOtaEvent used for parceling.
     *
     * @param in the parcel to read from
     */
    protected GattOtaEvent(Parcel in) {
        progress = in.readInt();
        desc = in.readString();
    }

    /**
     * Creator for GattOtaEvent used for parceling.
     */
    public static final Creator<GattOtaEvent> CREATOR = new Creator<GattOtaEvent>() {
        @Override
        public GattOtaEvent createFromParcel(Parcel in) {
            return new GattOtaEvent(in);
        }

        @Override
        public GattOtaEvent[] newArray(int size) {
            return new GattOtaEvent[size];
        }
    };

    /**
     * Gets the description of the event.
     *
     * @return the description
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Gets the progress of the OTA update.
     *
     * @return the progress
     */
    public int getProgress() {
        return progress;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(progress);
        dest.writeString(desc);
    }
}