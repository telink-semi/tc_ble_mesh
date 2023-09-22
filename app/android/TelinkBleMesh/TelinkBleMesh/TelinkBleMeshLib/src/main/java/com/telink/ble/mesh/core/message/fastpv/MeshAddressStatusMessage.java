/********************************************************************************************************
 * @file MeshAddressStatusMessage.java
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
package com.telink.ble.mesh.core.message.fastpv;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.util.Arrays;

/**
 * This class represents a Mesh Address Status Message, which is a type of Status Message in a mesh network.
 * It extends the StatusMessage class and implements the Parcelable interface for serialization.
 * The Mesh Address Status Message contains information about the MAC address and PID (Product ID) of a device.
 */
public class MeshAddressStatusMessage extends StatusMessage implements Parcelable {
    private byte[] mac; // The MAC address of the device
    private int pid; // The Product ID of the device

    /**
     * Default constructor for the MeshAddressStatusMessage class.
     */
    public MeshAddressStatusMessage() {
    }

    /**
     * Constructor for the MeshAddressStatusMessage class that takes a Parcel as input.
     * It is used for deserialization of the object.
     *
     * @param in The Parcel object to read the data from.
     */
    protected MeshAddressStatusMessage(Parcel in) {
        mac = in.createByteArray();
        pid = in.readInt();
    }

    /**
     * Creator constant for the MeshAddressStatusMessage class.
     * It is used to create new instances of the class from a Parcel.
     */
    public static final Creator<MeshAddressStatusMessage> CREATOR = new Creator<MeshAddressStatusMessage>() {
        @Override
        public MeshAddressStatusMessage createFromParcel(Parcel in) {
            return new MeshAddressStatusMessage(in);
        }

        @Override
        public MeshAddressStatusMessage[] newArray(int size) {
            return new MeshAddressStatusMessage[size];
        }
    };

    /**
     * Method to parse the byte array data and populate the object fields.
     *
     * @param params The byte array containing the data to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        final int macLen = 6;
        this.mac = new byte[6];
        System.arraycopy(params, 0, this.mac, 0, macLen);
        index += macLen;
        this.pid = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Method to describe the contents of the MeshAddressStatusMessage object.
     *
     * @return An integer value representing the contents of the object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Method to write the object data to a Parcel object for serialization.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags for writing the data.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByteArray(mac);
        dest.writeInt(pid);
    }

    /**
     * Getter method for the MAC address of the device.
     *
     * @return A byte array representing the MAC address.
     */
    public byte[] getMac() {
        return mac;
    }

    /**
     * Getter method for the Product ID of the device.
     *
     * @return An integer value representing the Product ID.
     */
    public int getPid() {
        return pid;
    }

    /**
     * Method to generate a string representation of the MeshAddressStatusMessage object.
     *
     * @return A string representation of the object.
     */
    @Override
    public String toString() {
        return "MeshAddressStatusMessage{" +
                "mac=" + Arrays.toString(mac) +
                ", pid=" + pid +
                '}';
    }
}