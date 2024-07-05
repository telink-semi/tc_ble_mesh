/********************************************************************************************************
 * @file SensorStatusMessage.java
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
import android.util.SparseArray;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteOrder;

/**
 * This class represents a status message for the Sensor Descriptor.
 * It extends the StatusMessage class and implements
 * the Parcelable interface for easy serialization and deserialization.
 */
public class SensorStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Bit field indicating defined Actions in the Schedule Register
     */
    public SparseArray<byte[]> sensorData;

    /**
     * Default constructor for the SchedulerStatusMessage class.
     */
    public SensorStatusMessage() {
    }


    /**
     * Constructor for the SchedulerStatusMessage class that takes a Parcel as a parameter for deserialization.
     *
     * @param in The Parcel object used for deserialization.
     */
    protected SensorStatusMessage(Parcel in) {
    }


    public static final Creator<SensorStatusMessage> CREATOR = new Creator<SensorStatusMessage>() {
        @Override
        public SensorStatusMessage createFromParcel(Parcel in) {
            return new SensorStatusMessage(in);
        }

        @Override
        public SensorStatusMessage[] newArray(int size) {
            return new SensorStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameter and sets the schedules field accordingly.
     *
     * @param params The byte array to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        sensorData = new SparseArray<>();
        int len;
        int property;
        byte[] data;
        for (int i = 0; i < params.length; ) {
            int b0 = params[i++];
            int format = b0 & 0x01;
            if (format == 0) {
                len = (b0 & 0x1F) >> 1;
                property = ((b0 & 0xFF) >> 5) | (params[i++] << 8);
            } else {
                len = (b0 & 0xFF) >> 1;
                if (len == 0x7F) {
                    len = 0;
                } else {
                    len = len + 1;
                }
                property = MeshUtils.bytes2Integer(params, i, 2, ByteOrder.LITTLE_ENDIAN);
                i += 2;
            }

            data = new byte[len];
            System.arraycopy(params, i, data, 0, data.length);
            i += len;
            MeshLogger.d(String.format("sensor status : %04X - ", property) + Arrays.bytesToHexString(data));
            sensorData.append(property, data);
        }
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
    }
}