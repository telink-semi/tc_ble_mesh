/********************************************************************************************************
 * @file CtlStatusMessage.java
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
package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a Control Status Message, which is a type of status message that contains information
 * about the present and target lightness and temperature values, as well as the remaining time for the transition.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer between components.
 */
public class CtlStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 9;
    private int presentLightness;
    private int presentTemperature;
    private int targetLightness;
    private int targetTemperature;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor for the CtlStatusMessage class.
     */
    public CtlStatusMessage() {
    }

    /**
     * Constructor for the CtlStatusMessage class that initializes its members based on the values from the given Parcel.
     *
     * @param in The Parcel containing the values for the members of the CtlStatusMessage.
     */
    protected CtlStatusMessage(Parcel in) {
        presentLightness = in.readInt();
        presentTemperature = in.readInt();
        targetLightness = in.readInt();
        targetTemperature = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator constant for the CtlStatusMessage class, used to create new instances of the class from a Parcel.
     */
    public static final Creator<CtlStatusMessage> CREATOR = new Creator<CtlStatusMessage>() {
        @Override
        public CtlStatusMessage createFromParcel(Parcel in) {
            return new CtlStatusMessage(in);
        }

        @Override
        public CtlStatusMessage[] newArray(int size) {
            return new CtlStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets the values of the members of the CtlStatusMessage accordingly.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.presentTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.targetTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.remainingTime = params[index];
        }
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentLightness);
        dest.writeInt(presentTemperature);
        dest.writeInt(targetLightness);
        dest.writeInt(targetTemperature);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Returns the present lightness value.
     *
     * @return The present lightness value.
     */
    public int getPresentLightness() {
        return presentLightness;
    }

    /**
     * Returns the present temperature value.
     *
     * @return The present temperature value.
     */
    public int getPresentTemperature() {
        return presentTemperature;
    }

    /**
     * Returns the target lightness value.
     *
     * @return The target lightness value.
     */
    public int getTargetLightness() {
        return targetLightness;
    }

    /**
     * Returns the target temperature value.
     *
     * @return The target temperature value.
     */
    public int getTargetTemperature() {
        return targetTemperature;
    }

    /**
     * Returns the remaining time for the transition.
     *
     * @return The remaining time for the transition.
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
     * Returns a string representation of the CtlStatusMessage object.
     *
     * @return A string representation of the CtlStatusMessage object.
     */
    @Override
    public String toString() {
        return "CtlStatusMessage{" +
                "presentLightness=" + presentLightness +
                ", presentTemperature=" + presentTemperature +
                ", targetLightness=" + targetLightness +
                ", targetTemperature=" + targetTemperature +
                ", remainingTime=" + remainingTime +
                ", isComplete=" + isComplete +
                '}';
    }
}