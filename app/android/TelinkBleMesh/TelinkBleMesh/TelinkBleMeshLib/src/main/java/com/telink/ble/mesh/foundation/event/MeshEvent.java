/********************************************************************************************************
 * @file MeshEvent.java
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
 * Represents a mesh event.
 * Extends the Event class with a String type.
 * Contains constants for different types of mesh events.
 * Stores a description of the event.
 */
public class MeshEvent extends Event<String> {
    // Constants for different types of mesh events
    public static final String EVENT_TYPE_MESH_EMPTY = "com.telink.ble.mesh.MESH_EMPTY";
    public static final String EVENT_TYPE_DISCONNECTED = "com.telink.ble.mesh.EVENT_TYPE_DISCONNECTED";
    public static final String EVENT_TYPE_MESH_RESET = "com.telink.sig.mesh.EVENT_TYPE_MESH_RESET";

    // Description of the event
    private String desc;

    /**
     * Constructs a MeshEvent object with the specified sender, type, and description.
     *
     * @param sender the object that sent the event
     * @param type   the type of the event
     * @param desc   the description of the event
     */
    public MeshEvent(Object sender, String type, String desc) {
        super(sender, type);
        this.desc = desc;
    }

    /**
     * Constructs a MeshEvent object from a Parcel.
     *
     * @param in the Parcel to read from
     */
    protected MeshEvent(Parcel in) {
        desc = in.readString();
    }

    /**
     * Creator for creating MeshEvent objects from a Parcel.
     */
    public static final Creator<MeshEvent> CREATOR = new Creator<MeshEvent>() {
        @Override
        public MeshEvent createFromParcel(Parcel in) {
            return new MeshEvent(in);
        }

        @Override
        public MeshEvent[] newArray(int size) {
            return new MeshEvent[size];
        }
    };

    /**
     * Gets the description of the event.
     *
     * @return the description of the event
     */
    public String getDesc() {
        return desc;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(desc);
    }
}
