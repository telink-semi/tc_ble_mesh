/********************************************************************************************************
 * @file     UnprovisionedDevice.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.sig.mesh.ble;

import android.bluetooth.BluetoothDevice;
import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by kee on 2018/7/19.
 */

public class UnprovisionedDevice implements Parcelable {
    public BluetoothDevice device;

    public int unicastAddress;

    public int rssi;
// mac:FF:FF:BB:CC:DD:76 rssi:-25
// record:  02:01:06:
// 03:03:27:18:
// 15:16:27:18:11:02:07:00:32:37:69:00:07:00:76:DD:CC:BB:FF:FF:00:00:1E:FF:76:DD:CC:BB:FF:FF:76:5D:
// 00:00:00:00:00:00:00:00:00:00:01:02:03:04:05:06:07:08:09:0A:0B:00:00
    public byte[] scanRecord;


    public UnprovisionedDevice() {

    }

    public UnprovisionedDevice(AdvertisingDevice device, int address) {
        this.device = device.device;
        this.rssi = device.rssi;
        this.scanRecord = device.scanRecord;
        this.unicastAddress = address;
    }

    protected UnprovisionedDevice(Parcel in) {
        device = in.readParcelable(BluetoothDevice.class.getClassLoader());
        unicastAddress = in.readInt();
        rssi = in.readInt();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(device, flags);
        dest.writeInt(unicastAddress);
        dest.writeInt(rssi);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<UnprovisionedDevice> CREATOR = new Creator<UnprovisionedDevice>() {
        @Override
        public UnprovisionedDevice createFromParcel(Parcel in) {
            return new UnprovisionedDevice(in);
        }

        @Override
        public UnprovisionedDevice[] newArray(int size) {
            return new UnprovisionedDevice[size];
        }
    };
}