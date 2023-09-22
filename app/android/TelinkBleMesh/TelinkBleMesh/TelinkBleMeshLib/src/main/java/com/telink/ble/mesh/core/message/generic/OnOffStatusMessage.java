/********************************************************************************************************
 * @file OnOffStatusMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a message for the on/off status of a device.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class OnOffStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 3;
    private byte presentOnOff;
    private byte targetOnOff;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor for the OnOffStatusMessage class.
     */
    public OnOffStatusMessage() {
    }

    /**
     * Constructor for the OnOffStatusMessage class that initializes its attributes from a Parcel.
     *
     * @param in The Parcel object to read the attributes from.
     */
    protected OnOffStatusMessage(Parcel in) {
        presentOnOff = in.readByte();
        targetOnOff = in.readByte();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     */
    public static final Creator<OnOffStatusMessage> CREATOR = new Creator<OnOffStatusMessage>() {
        @Override
        public OnOffStatusMessage createFromParcel(Parcel in) {
            return new OnOffStatusMessage(in);
        }

        @Override
        public OnOffStatusMessage[] newArray(int size) {
            return new OnOffStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array to initialize the attributes of the OnOffStatusMessage object.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        this.presentOnOff = params[0];
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetOnOff = params[1];
            this.remainingTime = params[2];
        }
    }

    /**
     * Returns the present on/off status.
     *
     * @return The present on/off status.
     */
    public byte getPresentOnOff() {
        return presentOnOff;
    }

    /**
     * Returns the target on/off status.
     *
     * @return The target on/off status.
     */
    public byte getTargetOnOff() {
        return targetOnOff;
    }

    /**
     * Returns the remaining time for the on/off status change.
     *
     * @return The remaining time for the on/off status change.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns whether the message is complete or not.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
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
     * Flattens the object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(presentOnOff);
        dest.writeByte(targetOnOff);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}