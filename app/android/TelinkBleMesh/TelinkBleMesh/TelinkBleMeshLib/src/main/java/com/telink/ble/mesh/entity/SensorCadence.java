/********************************************************************************************************
 * @file SensorCadence.java
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

import com.telink.ble.mesh.core.DeviceProperty;
import com.telink.ble.mesh.core.MeshUtils;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a model publication in the Bluetooth Mesh network.
 * publication entity used when send publicationSet message or receive publication status message
 * Created by kee on 2018/9/17.
 */
public final class SensorCadence implements Serializable, Parcelable {

    public int propertyID;

    public byte fastCadencePeriodDivisor;

    // 固件端默认为0， 也就是需要采用 device property 里面对应的设备属性 data len
    // 如果是1， 则传入的是两个字节的百分比
    public int statusTriggerType;

    public byte[] statusTriggerDeltaDown;

    public byte[] statusTriggerDeltaUp;

    public byte statusMinInterval;

    public byte[] fastCadenceLow;

    public byte[] fastCadenceHigh;


    protected SensorCadence(Parcel in) {
        propertyID = in.readInt();
        fastCadencePeriodDivisor = in.readByte();
        statusTriggerType = in.readInt();
        statusTriggerDeltaDown = in.createByteArray();
        statusTriggerDeltaUp = in.createByteArray();
        statusMinInterval = in.readByte();
        fastCadenceLow = in.createByteArray();
        fastCadenceHigh = in.createByteArray();
    }

    public static final Creator<SensorCadence> CREATOR = new Creator<SensorCadence>() {
        @Override
        public SensorCadence createFromParcel(Parcel in) {
            return new SensorCadence(in);
        }

        @Override
        public SensorCadence[] newArray(int size) {
            return new SensorCadence[size];
        }
    };

    public static SensorCadence fromBytes(byte[] params) {
        SensorCadence instance = new SensorCadence();
        int index = 0;
        instance.propertyID = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        instance.fastCadencePeriodDivisor = (byte) (params[index] & 0x7F);

        instance.statusTriggerType = (params[index] >> 7) & 0x01;
        int len = 1;
        int propLen = 1;
        if (instance.statusTriggerType == 0) {
            if (instance.propertyID == DeviceProperty.PRESENT_AMBIENT_LIGHT_LEVEL.id) {
                propLen = DeviceProperty.PRESENT_AMBIENT_LIGHT_LEVEL.len;
            } else if (instance.propertyID == DeviceProperty.MOTION_SENSED.id) {
                propLen = DeviceProperty.MOTION_SENSED.len;
            } //  else len = 1

            len = propLen;
        } else {
            // 1, use u16 to parse delta down and delta up
            propLen = 2;
        }
        instance.statusTriggerDeltaDown = new byte[len];
        System.arraycopy(params, index, instance.statusTriggerDeltaDown, 0, len);
        index += len;
        instance.statusTriggerDeltaUp = new byte[len];
        System.arraycopy(params, index, instance.statusTriggerDeltaUp, 0, len);
        index += len;


        instance.statusMinInterval = params[index++];

        instance.fastCadenceLow = new byte[propLen];
        System.arraycopy(params, index, instance.fastCadenceLow, 0, propLen);
        index += len;
        instance.fastCadenceHigh = new byte[propLen];
        System.arraycopy(params, index, instance.fastCadenceHigh, 0, propLen);
        return instance;
    }

    /**
     * Constructs a new ModelPublication object.
     * This is used for creating a default instance of the class.
     */
    public SensorCadence() {
    }


    /**
     * Converts the ModelPublication object to a byte array.
     *
     * @return The byte array representation of the ModelPublication object.
     */
    public byte[] toBytes() {
        int len = 2 + 1 + statusTriggerDeltaDown.length + statusTriggerDeltaUp.length + 1 + fastCadenceLow.length + fastCadenceHigh.length;
        return ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) propertyID).put((byte) (fastCadencePeriodDivisor | (statusTriggerType << 7)))
                .put(statusTriggerDeltaDown)
                .put(statusTriggerDeltaUp)
                .put(statusMinInterval)
                .put(fastCadenceLow)
                .put(fastCadenceHigh).array();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(propertyID);
        dest.writeByte(fastCadencePeriodDivisor);
        dest.writeInt(statusTriggerType);
        dest.writeByteArray(statusTriggerDeltaDown);
        dest.writeByteArray(statusTriggerDeltaUp);
        dest.writeByte(statusMinInterval);
        dest.writeByteArray(fastCadenceLow);
        dest.writeByteArray(fastCadenceHigh);
    }
}
