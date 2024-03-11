/********************************************************************************************************
 * @file ModelPublication.java
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

package com.telink.ble.mesh.entity;


import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a model publication in the Bluetooth Mesh network.
 * publication entity used when send publicationSet message or receive publication status message
 * Created by kee on 2018/9/17.
 */
public final class SensorDescriptor implements Serializable, Parcelable {

    public static final int DATA_LEN = 8;

    /**
     * Property ID
     * 16 bits
     */
    public int propertyID;

    /**
     * Value of the Positive Tolerance
     * 12 bits
     */
    public int positiveTolerance;

    /**
     * Value of the Negative Tolerance
     * 12 bits
     */
    public int negativeTolerance;


    /**
     * Value of the Sampling Function
     * 8 bits
     */
    public byte samplingFunction;

    /**
     * Value of the Measurement Period
     * 8 bits
     */
    public byte measurementPeriod;


    /**
     * Value of the Update Interval
     * 8 bits
     */
    public byte updateInterval;


    protected SensorDescriptor(Parcel in) {
        propertyID = in.readInt();
        positiveTolerance = in.readInt();
        negativeTolerance = in.readInt();
        samplingFunction = in.readByte();
        measurementPeriod = in.readByte();
        updateInterval = in.readByte();
    }

    public static final Creator<SensorDescriptor> CREATOR = new Creator<SensorDescriptor>() {
        @Override
        public SensorDescriptor createFromParcel(Parcel in) {
            return new SensorDescriptor(in);
        }

        @Override
        public SensorDescriptor[] newArray(int size) {
            return new SensorDescriptor[size];
        }
    };

    public static SensorDescriptor fromBytes(byte[] data) {
        if (data.length < DATA_LEN) {
            return null;
        }
        int index = 0;
        SensorDescriptor instance = new SensorDescriptor();
        instance.propertyID = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        int tolerance = MeshUtils.bytes2Integer(data, index, 3, ByteOrder.LITTLE_ENDIAN);

        instance.positiveTolerance = tolerance & 0x0FFF;
        instance.negativeTolerance = (tolerance >> 12) & 0x0FFF;
        index += 3;
        instance.samplingFunction = data[index++];
        instance.measurementPeriod = data[index++];
        instance.updateInterval = data[index];
        return instance;
    }

    /**
     * Constructs a new ModelPublication object.
     * This is used for creating a default instance of the class.
     */
    public SensorDescriptor() {
    }


    /**
     * Converts the ModelPublication object to a byte array.
     *
     * @return The byte array representation of the ModelPublication object.
     */
    public byte[] toBytes() {
        int tolerance = (positiveTolerance & 0x0FFF) | ((negativeTolerance & 0x0FFF) << 12);
        byte[] toleranceBuf = MeshUtils.integer2Bytes(tolerance, 3, ByteOrder.LITTLE_ENDIAN);

        return ByteBuffer.allocate(DATA_LEN).order(ByteOrder.LITTLE_ENDIAN).putShort((short) propertyID)
                .put(toleranceBuf)
                .put(samplingFunction)
                .put(measurementPeriod)
                .put(updateInterval).array()
                ;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(propertyID);
        dest.writeInt(positiveTolerance);
        dest.writeInt(negativeTolerance);
        dest.writeByte(samplingFunction);
        dest.writeByte(measurementPeriod);
        dest.writeByte(updateInterval);
    }
}
