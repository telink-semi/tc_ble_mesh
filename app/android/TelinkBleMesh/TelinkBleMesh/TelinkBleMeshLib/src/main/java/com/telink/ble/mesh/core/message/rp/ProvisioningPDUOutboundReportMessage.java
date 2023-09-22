/********************************************************************************************************
 * @file ProvisioningPDUOutboundReportMessage.java
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

/**
 * This class represents a ProvisioningPDUOutboundReportMessage, which is a type of StatusMessage that is used to report the number of outbound PDUs.
 * It implements the Parcelable interface to allow for easy serialization and deserialization of the object.
 */
public class ProvisioningPDUOutboundReportMessage extends StatusMessage implements Parcelable {
    private byte outboundPDUNumber;

    /**
     * Default constructor for the ProvisioningPDUOutboundReportMessage class.
     */
    public ProvisioningPDUOutboundReportMessage() {
    }

    /**
     * Constructor for the ProvisioningPDUOutboundReportMessage class that takes a Parcel as input and initializes the outboundPDUNumber field.
     *
     * @param in The Parcel object used for deserialization.
     */
    protected ProvisioningPDUOutboundReportMessage(Parcel in) {
        outboundPDUNumber = in.readByte();
    }

    /**
     * Creator constant for the ProvisioningPDUOutboundReportMessage class, used to create new instances of the class from a Parcel.
     */
    public static final Creator<ProvisioningPDUOutboundReportMessage> CREATOR = new Creator<ProvisioningPDUOutboundReportMessage>() {
        @Override
        public ProvisioningPDUOutboundReportMessage createFromParcel(Parcel in) {
            return new ProvisioningPDUOutboundReportMessage(in);
        }

        @Override
        public ProvisioningPDUOutboundReportMessage[] newArray(int size) {
            return new ProvisioningPDUOutboundReportMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets the outboundPDUNumber field.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        this.outboundPDUNumber = params[0];
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the outboundPDUNumber field to the given Parcel object.
     *
     * @param dest  The Parcel object to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(outboundPDUNumber);
    }

    /**
     * Returns the number of outbound PDUs.
     *
     * @return The number of outbound PDUs.
     */
    public byte getOutboundPDUNumber() {
        return outboundPDUNumber;
    }
}