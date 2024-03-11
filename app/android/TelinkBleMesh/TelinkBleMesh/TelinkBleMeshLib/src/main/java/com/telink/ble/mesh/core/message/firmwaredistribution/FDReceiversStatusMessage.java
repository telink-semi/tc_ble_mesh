/********************************************************************************************************
 * @file FDReceiversStatusMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a status message for firmware distribution receivers.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer.
 * The status code and the number of entries in the distribution receivers list are stored as instance variables.
 */
public class FDReceiversStatusMessage extends StatusMessage implements Parcelable {
    /**
     * Status Code for the requesting message
     * 1 byte
     */
    private int status;
    /**
     * Receivers List Count
     * The number of entries in the Distribution Receivers List state
     * 2 bytes
     */
    private int receiversListCount;

    /**
     * Default constructor for the FDReceiversStatusMessage class.
     */
    public FDReceiversStatusMessage() {
    }

    /**
     * Constructor for the FDReceiversStatusMessage class that initializes the instance variables from a Parcel.
     *
     * @param in The Parcel containing the status code and receivers list count.
     */
    protected FDReceiversStatusMessage(Parcel in) {
        status = in.readInt();
        receiversListCount = in.readInt();
    }

    /**
     * Creator constant for the FDReceiversStatusMessage class.
     */
    public static final Creator<FDReceiversStatusMessage> CREATOR = new Creator<FDReceiversStatusMessage>() {
        @Override
        public FDReceiversStatusMessage createFromParcel(Parcel in) {
            return new FDReceiversStatusMessage(in);
        }

        @Override
        public FDReceiversStatusMessage[] newArray(int size) {
            return new FDReceiversStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to extract the status code and receivers list count.
     *
     * @param params The byte array containing the status code and receivers list count.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++] & 0xFF;
        this.receiversListCount = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Returns the status code.
     *
     * @return The status code.
     */
    public int getStatus() {
        return status;
    }

    /**
     * Returns the number of entries in the distribution receivers list.
     *
     * @return The number of entries in the distribution receivers list.
     */
    public int getReceiversListCount() {
        return receiversListCount;
    }

    /**
     * Implements the describeContents() method from the Parcelable interface.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the status code and receivers list count to the Parcel.
     *
     * @param dest  The Parcel to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(receiversListCount);
    }

    /**
     * Returns a string representation of the FDReceiversStatusMessage object.
     *
     * @return A string representation of the FDReceiversStatusMessage object.
     */
    @Override
    public String toString() {
        return "FirmwareDistributionReceiversStatus{" +
                "status=" + status +
                ", receiversListCount=" + receiversListCount +
                '}';
    }
}