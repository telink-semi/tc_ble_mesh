/********************************************************************************************************
 * @file RemoteProvisioningDevice.java
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

/**
 * Model for provisioning flow
 * Created by kee on 2019/9/4.
 */
// advertisingDevice is null
public class RemoteProvisioningDevice extends ProvisioningDevice {

    private byte rssi;

    private byte[] uuid = null;

    // proxy address
    private int serverAddress;

    public RemoteProvisioningDevice(byte rssi, byte[] uuid, int serverAddress) {
        this.rssi = rssi;
        this.uuid = uuid;
        this.serverAddress = serverAddress;
    }

    protected RemoteProvisioningDevice(Parcel in) {
        super(in);
        rssi = in.readByte();
        uuid = in.createByteArray();
        serverAddress = in.readInt();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        super.writeToParcel(dest, flags);
        dest.writeByte(rssi);
        dest.writeByteArray(uuid);
        dest.writeInt(serverAddress);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<RemoteProvisioningDevice> CREATOR = new Creator<RemoteProvisioningDevice>() {
        @Override
        public RemoteProvisioningDevice createFromParcel(Parcel in) {
            return new RemoteProvisioningDevice(in);
        }

        @Override
        public RemoteProvisioningDevice[] newArray(int size) {
            return new RemoteProvisioningDevice[size];
        }
    };

    public byte getRssi() {
        return rssi;
    }

    public byte[] getUuid() {
        return uuid;
    }

    public int getServerAddress() {
        return serverAddress;
    }

    public void setRssi(byte rssi) {
        this.rssi = rssi;
    }

    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }

    public void setServerAddress(int serverAddress) {
        this.serverAddress = serverAddress;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        RemoteProvisioningDevice device = (RemoteProvisioningDevice) o;
        return java.util.Arrays.equals(uuid, device.uuid);
    }

    @Override
    public int hashCode() {
        return java.util.Arrays.hashCode(uuid);
    }


}
