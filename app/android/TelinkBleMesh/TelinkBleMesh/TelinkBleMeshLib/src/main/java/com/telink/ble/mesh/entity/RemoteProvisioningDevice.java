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
 * This class represents a remote provisioning device, which extends the ProvisioningDevice class.
 * It contains information such as RSSI (Received Signal Strength Indicator), UUID (Universally Unique Identifier),
 * and server address.
 */
public class RemoteProvisioningDevice extends ProvisioningDevice {
    private byte rssi;
    private byte[] uuid = null;
    private int serverAddress;

    /**
     * Constructs a RemoteProvisioningDevice object with the specified RSSI, UUID, and server address.
     *
     * @param rssi          The RSSI value of the device.
     * @param uuid          The UUID of the device.
     * @param serverAddress The server address of the device.
     */
    public RemoteProvisioningDevice(byte rssi, byte[] uuid, int serverAddress) {
        this.rssi = rssi;
        this.uuid = uuid;
        this.serverAddress = serverAddress;
    }

    /**
     * Constructs a RemoteProvisioningDevice object from a Parcel.
     *
     * @param in The Parcel object to read the data from.
     */
    protected RemoteProvisioningDevice(Parcel in) {
        super(in);
        rssi = in.readByte();
        uuid = in.createByteArray();
        serverAddress = in.readInt();
    }

    /**
     * Writes the data of the RemoteProvisioningDevice object to a Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        super.writeToParcel(dest, flags);
        dest.writeByte(rssi);
        dest.writeByteArray(uuid);
        dest.writeInt(serverAddress);
    }

    /**
     * Describes the contents of the RemoteProvisioningDevice object.
     *
     * @return An integer value representing the contents of the object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     * Used to create instances of RemoteProvisioningDevice from a Parcel.
     */
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

    /**
     * Gets the RSSI value of the device.
     *
     * @return The RSSI value.
     */
    public byte getRssi() {
        return rssi;
    }

    /**
     * Gets the UUID of the device.
     *
     * @return The UUID.
     */
    public byte[] getUuid() {
        return uuid;
    }

    /**
     * Gets the server address of the device.
     *
     * @return The server address.
     */
    public int getServerAddress() {
        return serverAddress;
    }

    /**
     * Sets the RSSI value of the device.
     *
     * @param rssi The RSSI value to set.
     */
    public void setRssi(byte rssi) {
        this.rssi = rssi;
    }

    /**
     * Sets the UUID of the device.
     *
     * @param uuid The UUID to set.
     */
    public void setUuid(byte[] uuid) {
        this.uuid = uuid;
    }

    /**
     * Sets the server address of the device.
     *
     * @param serverAddress The server address to set.
     */
    public void setServerAddress(int serverAddress) {
        this.serverAddress = serverAddress;
    }

    /**
     * Checks if this RemoteProvisioningDevice object is equal to another object.
     *
     * @param o The object to compare to.
     * @return True if the objects are equal, false otherwise.
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        RemoteProvisioningDevice device = (RemoteProvisioningDevice) o;
        return java.util.Arrays.equals(uuid, device.uuid);
    }

    /**
     * Generates a hash code for the RemoteProvisioningDevice object.
     *
     * @return The hash code value.
     */
    @Override
    public int hashCode() {
        return java.util.Arrays.hashCode(uuid);
    }
}
