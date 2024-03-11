/********************************************************************************************************
 * @file LevelStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/9/3.
 */

/**
 * This class represents a status message for the Generic Level state in a mesh network.
 * It extends the StatusMessage class.
 * The status message includes the present level value, target level value (optional),
 * remaining time, and a flag indicating if the message is complete.
 * The present level and target level values are represented as integers.
 * The remaining time is represented as a byte.
 */
public class LevelStatusMessage extends StatusMessage {
    private static final int DATA_LEN_COMPLETE = 5;
    private int presentLevel;
    private int targetLevel;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor.
     */
    public LevelStatusMessage() {
    }

    /**
     * Constructor for creating a LevelStatusMessage object from a Parcel.
     *
     * @param in The Parcel object to read the values from.
     */
    protected LevelStatusMessage(Parcel in) {
        presentLevel = in.readInt();
        targetLevel = in.readInt();
        remainingTime = in.readByte();
    }

    /**
     * Creator object for creating LevelStatusMessage objects from a Parcel.
     */
    public static final Creator<LevelStatusMessage> CREATOR = new Creator<LevelStatusMessage>() {
        @Override
        public LevelStatusMessage createFromParcel(Parcel in) {
            return new LevelStatusMessage(in);
        }

        @Override
        public LevelStatusMessage[] newArray(int size) {
            return new LevelStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the values of the present level, target level (if available),
     * remaining time, and the completion flag.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentLevel = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLevel = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.remainingTime = params[index];
        }
    }

    /**
     * Returns the description of the contents of this object.
     *
     * @return The description of the contents.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the values of the present level, target level, and remaining time to the Parcel.
     *
     * @param dest  The Parcel to write the values to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentLevel);
        dest.writeInt(targetLevel);
        dest.writeByte(remainingTime);
    }

    /**
     * Returns the present level value.
     *
     * @return The present level value.
     */
    public int getPresentLevel() {
        return presentLevel;
    }

    /**
     * Returns the target level value.
     *
     * @return The target level value.
     */
    public int getTargetLevel() {
        return targetLevel;
    }

    /**
     * Returns the remaining time value.
     *
     * @return The remaining time value.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns a flag indicating if the status message is complete.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }
}