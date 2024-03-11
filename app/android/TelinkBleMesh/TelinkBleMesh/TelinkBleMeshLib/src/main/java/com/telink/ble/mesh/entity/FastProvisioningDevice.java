/********************************************************************************************************
 * @file FastProvisioningDevice.java
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

import com.telink.ble.mesh.util.Arrays;

/**
 * This class represents a fast provisioning device.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 */
public class FastProvisioningDevice implements Parcelable {
    /**
     * the original address of the device
     */
    private int originAddress;
    /**
     * the new address of the device
     */
    private int newAddress;

    /**
     * the product ID of the device
     */
    private int pid;

    /**
     * the number of elements in the device
     */
    private int elementCount;

    /**
     * the MAC address of the device
     */
    private byte[] mac;

    /**
     * the device key of the device
     */
    private byte[] deviceKey;


    public FastProvisioningDevice() {

    }

    public FastProvisioningDevice(int originAddress, int newAddress, int pid, int elementCount, byte[] mac) {
        this.originAddress = originAddress;
        this.newAddress = newAddress;
        this.pid = pid;
        this.elementCount = elementCount;
        this.mac = Arrays.reverse(mac);
        this.deviceKey = new byte[16];
        System.arraycopy(mac, 0, deviceKey, 0, 6);
    }

    protected FastProvisioningDevice(Parcel in) {
        originAddress = in.readInt();
        newAddress = in.readInt();
        pid = in.readInt();
        elementCount = in.readInt();
        mac = in.createByteArray();
        deviceKey = in.createByteArray();
    }

    public static final Creator<FastProvisioningDevice> CREATOR = new Creator<FastProvisioningDevice>() {
        @Override
        public FastProvisioningDevice createFromParcel(Parcel in) {
            return new FastProvisioningDevice(in);
        }

        @Override
        public FastProvisioningDevice[] newArray(int size) {
            return new FastProvisioningDevice[size];
        }
    };

    public byte[] getMac() {
        return mac;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        FastProvisioningDevice device = (FastProvisioningDevice) o;
        return java.util.Arrays.equals(mac, device.mac);
    }

    @Override
    public int hashCode() {
        return java.util.Arrays.hashCode(mac);
    }

    public int getOriginAddress() {
        return originAddress;
    }

    public void setOriginAddress(int originAddress) {
        this.originAddress = originAddress;
    }

    public int getNewAddress() {
        return newAddress;
    }

    public void setNewAddress(int newAddress) {
        this.newAddress = newAddress;
    }

    public int getPid() {
        return pid;
    }

    public void setPid(int pid) {
        this.pid = pid;
    }

    public void setMac(byte[] mac) {
        this.mac = mac;
    }

    public byte[] getDeviceKey() {
        return deviceKey;
    }

    public void setDeviceKey(byte[] deviceKey) {
        this.deviceKey = deviceKey;
    }

    public int getElementCount() {
        return elementCount;
    }

    public void setElementCount(int elementCount) {
        this.elementCount = elementCount;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(originAddress);
        dest.writeInt(newAddress);
        dest.writeInt(pid);
        dest.writeInt(elementCount);
        dest.writeByteArray(mac);
        dest.writeByteArray(deviceKey);
    }

    @Override
    public String toString() {
        return "FastProvisioningDevice{" +
                "originAddress=" + originAddress +
                ", newAddress=" + newAddress +
                ", pid=" + pid +
                ", elementCount=" + elementCount +
                ", mac=" + java.util.Arrays.toString(mac) +
                ", deviceKey=" + java.util.Arrays.toString(deviceKey) +
                '}';
    }
}
