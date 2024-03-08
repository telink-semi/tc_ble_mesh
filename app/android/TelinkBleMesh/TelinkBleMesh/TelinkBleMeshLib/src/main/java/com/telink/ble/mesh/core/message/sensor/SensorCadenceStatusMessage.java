/********************************************************************************************************
 * @file SensorDescriptorStatusMessage.java
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
package com.telink.ble.mesh.core.message.sensor;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.entity.SensorCadence;

/**
 * This class represents a status message for the Sensor Descriptor.
 * It extends the StatusMessage class and implements
 * the Parcelable interface for easy serialization and deserialization.
 */
public class SensorCadenceStatusMessage extends StatusMessage implements Parcelable {

    public SensorCadence cadence;


    /**
     * Default constructor for the SchedulerStatusMessage class.
     */
    public SensorCadenceStatusMessage() {
    }


    /**
     * Creator constant for the Parcelable interface.
     */

    protected SensorCadenceStatusMessage(Parcel in) {
        cadence = in.readParcelable(SensorCadence.class.getClassLoader());
    }

    /**
     * Constructor for the SchedulerStatusMessage class that takes a Parcel as a parameter for deserialization.
     *
     * @param in The Parcel object used for deserialization.
     */
    public static final Creator<SensorCadenceStatusMessage> CREATOR = new Creator<SensorCadenceStatusMessage>() {
        @Override
        public SensorCadenceStatusMessage createFromParcel(Parcel in) {
            return new SensorCadenceStatusMessage(in);
        }

        @Override
        public SensorCadenceStatusMessage[] newArray(int size) {
            return new SensorCadenceStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameter and sets the schedules field accordingly.
     *
     * @param params The byte array to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        cadence = SensorCadence.fromBytes(params);
    }

    /**
     * Describes the contents of the Parcelable object.
     *
     * @return An integer representing the contents.
     */

    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the object's data to the specified Parcel object.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(cadence, flags);
    }


}