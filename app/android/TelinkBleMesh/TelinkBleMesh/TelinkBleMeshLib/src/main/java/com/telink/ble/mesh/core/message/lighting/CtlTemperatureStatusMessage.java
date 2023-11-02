/********************************************************************************************************
 * @file CtlTemperatureStatusMessage.java
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
 * This class represents a message containing the status of the temperature control.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer.
 * The message contains information about the present temperature, present delta UV, target temperature,
 * target delta UV, and remaining time.
 */
public class CtlTemperatureStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 9;
    private int presentTemperature;
    private int presentDeltaUV;
    private int targetTemperature;
    private int targetDeltaUV;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor for CtlTemperatureStatusMessage.
     */
    public CtlTemperatureStatusMessage() {
    }

    /**
     * Constructor for CtlTemperatureStatusMessage that takes a Parcel as input and initializes the object.
     *
     * @param in The Parcel object containing the data to initialize the object.
     */
    protected CtlTemperatureStatusMessage(Parcel in) {
        presentTemperature = in.readInt();
        presentDeltaUV = in.readInt();
        targetTemperature = in.readInt();
        targetDeltaUV = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator constant for Parcelable interface.
     */
    public static final Creator<CtlTemperatureStatusMessage> CREATOR = new Creator<CtlTemperatureStatusMessage>() {
        @Override
        public CtlTemperatureStatusMessage createFromParcel(Parcel in) {
            return new CtlTemperatureStatusMessage(in);
        }

        @Override
        public CtlTemperatureStatusMessage[] newArray(int size) {
            return new CtlTemperatureStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and extracts the relevant data to populate the object.
     *
     * @param params The byte array containing the data to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.presentDeltaUV = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.targetDeltaUV = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.remainingTime = params[index];
        }
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
     * Writes the object's data to the given Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentTemperature);
        dest.writeInt(presentDeltaUV);
        dest.writeInt(targetTemperature);
        dest.writeInt(targetDeltaUV);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Returns the present temperature.
     *
     * @return The present temperature.
     */
    public int getPresentTemperature() {
        return presentTemperature;
    }

    /**
     * Returns the present delta UV.
     *
     * @return The present delta UV.
     */
    public int getPresentDeltaUV() {
        return presentDeltaUV;
    }

    /**
     * Returns the target temperature.
     *
     * @return The target temperature.
     */
    public int getTargetTemperature() {
        return targetTemperature;
    }

    /**
     * Returns the target delta UV.
     *
     * @return The target delta UV.
     */
    public int getTargetDeltaUV() {
        return targetDeltaUV;
    }

    /**
     * Returns the remaining time.
     *
     * @return The remaining time.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns true if the message is complete, false otherwise.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }
}