/********************************************************************************************************
 * @file SchedulerActionStatusMessage.java
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
package com.telink.ble.mesh.core.message.scheduler;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a status message for a scheduler action.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * It contains a byte array to store the scheduler parameters.
 *
 * @since 2019-08-20
 */
public class SchedulerActionStatusMessage extends StatusMessage implements Parcelable {
    private byte[] schedulerParams;

    /**
     * Default constructor for the SchedulerActionStatusMessage class.
     */
    public SchedulerActionStatusMessage() {
    }

    /**
     * Constructor for the SchedulerActionStatusMessage class that takes a Parcel as input.
     *
     * @param in The Parcel object used for deserialization.
     */
    protected SchedulerActionStatusMessage(Parcel in) {
        schedulerParams = in.createByteArray();
    }

    /**
     * Creator constant for the SchedulerActionStatusMessage class, used to generate instances of the class from a Parcel.
     */
    public static final Creator<SchedulerActionStatusMessage> CREATOR = new Creator<SchedulerActionStatusMessage>() {
        @Override
        public SchedulerActionStatusMessage createFromParcel(Parcel in) {
            return new SchedulerActionStatusMessage(in);
        }

        @Override
        public SchedulerActionStatusMessage[] newArray(int size) {
            return new SchedulerActionStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets it as the scheduler parameters.
     *
     * @param params The byte array representing the scheduler parameters.
     */
    @Override
    public void parse(byte[] params) {
        this.schedulerParams = params;
    }

    /**
     * Returns the scheduler parameters.
     *
     * @return The byte array representing the scheduler parameters.
     */
    public byte[] getSchedulerParams() {
        return schedulerParams;
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens this object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByteArray(schedulerParams);
    }
}