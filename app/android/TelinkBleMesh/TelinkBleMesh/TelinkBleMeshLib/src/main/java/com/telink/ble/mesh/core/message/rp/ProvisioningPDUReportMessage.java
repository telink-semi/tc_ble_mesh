/********************************************************************************************************
 * @file ProvisioningPDUReportMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.Arrays;

/**
 * This class represents a Provisioning PDU Report Message, which is a type of Status Message.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 * The Provisioning PDU Report Message contains an inbound PDU number and a byte array representing the provisioning PDU.
 */
public class ProvisioningPDUReportMessage extends StatusMessage implements Parcelable {
    private byte inboundPDUNumber;
    private byte[] provisioningPDU;

    /**
     * Default constructor for the ProvisioningPDUReportMessage class.
     */
    public ProvisioningPDUReportMessage() {
    }

    /**
     * Constructor for the ProvisioningPDUReportMessage class that takes a Parcel object as input.
     * It reads the inbound PDU number and provisioning PDU from the Parcel.
     *
     * @param in The Parcel object to read from.
     */
    protected ProvisioningPDUReportMessage(Parcel in) {
        inboundPDUNumber = in.readByte();
        provisioningPDU = in.createByteArray();
    }

    /**
     * Creator constant for the ProvisioningPDUReportMessage class.
     * It is used to create new instances of the class from a Parcel.
     */
    public static final Creator<ProvisioningPDUReportMessage> CREATOR = new Creator<ProvisioningPDUReportMessage>() {
        @Override
        public ProvisioningPDUReportMessage createFromParcel(Parcel in) {
            return new ProvisioningPDUReportMessage(in);
        }

        @Override
        public ProvisioningPDUReportMessage[] newArray(int size) {
            return new ProvisioningPDUReportMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the inbound PDU number and provisioning PDU.
     * If the byte array is longer than 1, it creates a new byte array for the provisioning PDU
     * and copies the bytes from the input array starting from index 1.
     *
     * @param params The byte array parameters to parse.
     */
    @Override
    public void parse(byte[] params) {
        this.inboundPDUNumber = params[0];
        if (params.length > 1) {
            int pduLen = params.length - 1;
            provisioningPDU = new byte[pduLen];
            System.arraycopy(params, 1, this.provisioningPDU, 0, pduLen);
        }
    }

    /**
     * Returns a bitmask indicating the set of special object types
     * marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the inbound PDU number and provisioning PDU to the Parcel.
     *
     * @param dest  The Parcel object to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(inboundPDUNumber);
        dest.writeByteArray(provisioningPDU);
    }

    /**
     * Returns the inbound PDU number.
     *
     * @return The inbound PDU number.
     */
    public byte getInboundPDUNumber() {
        return inboundPDUNumber;
    }

    /**
     * Returns the provisioning PDU.
     *
     * @return The provisioning PDU.
     */
    public byte[] getProvisioningPDU() {
        return provisioningPDU;
    }

    /**
     * Returns a string representation of the ProvisioningPDUReportMessage object.
     *
     * @return A string representation of the object.
     */
    @Override
    public String toString() {
        return "ProvisioningPDUReportMessage{" +
                "inboundPDUNumber=" + inboundPDUNumber +
                ", provisioningPDU=" + Arrays.bytesToHexString(provisioningPDU) +
                '}';
    }
}