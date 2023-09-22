/********************************************************************************************************
 * @file ScanStatusMessage.java
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
 * This class represents a Scan Status Message, which is a type of Status Message.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 * The Scan Status Message contains information about the status of a scan, including the current status,
 * scanning state, scanned items limit, and timeout.
 * The class provides methods to set and retrieve these values, as well as a parse method to extract the values from a byte array.
 */

public class ScanStatusMessage extends StatusMessage implements Parcelable {
    private byte status;
    private byte rpScanningState;
    private byte scannedItemsLimit;
    private byte timeout;

    /**
     * Default constructor for the ScanStatusMessage class.
     */
    public ScanStatusMessage() {
    }

    /**
     * Constructor for the ScanStatusMessage class that takes a Parcel as input.
     * It reads the values from the Parcel and assigns them to the corresponding fields.
     *
     * @param in The Parcel containing the values for the ScanStatusMessage.
     */
    protected ScanStatusMessage(Parcel in) {
        status = in.readByte();
        rpScanningState = in.readByte();
        scannedItemsLimit = in.readByte();
        timeout = in.readByte();
    }

    /**
     * Writes the values of the ScanStatusMessage to the Parcel.
     *
     * @param dest  The Parcel to write the values to.
     * @param flags Flags for writing the Parcel.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeByte(rpScanningState);
        dest.writeByte(scannedItemsLimit);
        dest.writeByte(timeout);
    }

    /**
     * Returns additional information about the contents of the ScanStatusMessage.
     *
     * @return An integer representing additional information about the contents of the ScanStatusMessage.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Creator object that implements the Parcelable.Creator interface.
     * It is used to create instances of the ScanStatusMessage class from a Parcel.
     */
    public static final Creator<ScanStatusMessage> CREATOR = new Creator<ScanStatusMessage>() {
        /**
         * Creates a new instance of the ScanStatusMessage class, instantiating it from the given Parcel.
         *
         * @param in The Parcel containing the values for the ScanStatusMessage.
         * @return The created instance of the ScanStatusMessage class.
         */
        @Override
        public ScanStatusMessage createFromParcel(Parcel in) {
            return new ScanStatusMessage(in);
        }

        /**
         * Creates a new array of the ScanStatusMessage class.
         *
         * @param size The size of the array to create.
         * @return An array of the ScanStatusMessage class.
         */
        @Override
        public ScanStatusMessage[] newArray(int size) {
            return new ScanStatusMessage[size];
        }
    };

    /**
     * Parses the values from a byte array and assigns them to the corresponding fields of the ScanStatusMessage.
     *
     * @param params The byte array containing the values to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.rpScanningState = params[index++];
        this.scannedItemsLimit = params[index++];
        this.timeout = params[index];
    }

    /**
     * Returns the status of the scan.
     *
     * @return The status of the scan.
     */
    public byte getStatus() {
        return status;
    }

    /**
     * Returns the scanning state of the scan.
     *
     * @return The scanning state of the scan.
     */
    public byte getRpScanningState() {
        return rpScanningState;
    }

    /**
     * Returns the limit of scanned items.
     *
     * @return The limit of scanned items.
     */
    public byte getScannedItemsLimit() {
        return scannedItemsLimit;
    }

    /**
     * Returns the timeout value.
     *
     * @return The timeout value.
     */
    public byte getTimeout() {
        return timeout;
    }
}