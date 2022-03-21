/********************************************************************************************************
 * @file AdvertisingDevice.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

import android.bluetooth.BluetoothDevice;
import android.os.Parcel;
import android.os.Parcelable;

import java.util.Arrays;

/**
 * scanned devices
 */
public class AdvertisingDevice implements Parcelable {
    public BluetoothDevice device;
    public int rssi;
    public byte[] scanRecord;

    public AdvertisingDevice(BluetoothDevice device, int rssi, byte[] scanRecord) {
        this.device = device;
        this.rssi = rssi;
        this.scanRecord = scanRecord;
    }

    public static final Creator<AdvertisingDevice> CREATOR = new Creator<AdvertisingDevice>() {
        @Override
        public AdvertisingDevice createFromParcel(Parcel in) {
            return new AdvertisingDevice(in);
        }

        @Override
        public AdvertisingDevice[] newArray(int size) {
            return new AdvertisingDevice[size];
        }
    };

    public AdvertisingDevice(Parcel in) {
        this.device = in.readParcelable(getClass().getClassLoader());
        this.rssi = in.readInt();
        this.scanRecord = new byte[in.readInt()];
        in.readByteArray(this.scanRecord);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(this.device, 0);
        dest.writeInt(this.rssi);
        dest.writeInt(this.scanRecord.length);
        dest.writeByteArray(this.scanRecord);
    }

    @Override
    public boolean equals(Object obj) {
        return obj instanceof AdvertisingDevice
                && ((AdvertisingDevice) obj).device.equals(device)
                && Arrays.equals(((AdvertisingDevice) obj).scanRecord, scanRecord);
    }

    @Override
    public int hashCode() {
        return device.hashCode();
    }
}
