/********************************************************************************************************
 * @file FirmwareMetadataStatusMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a firmware metadata status message.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class FirmwareMetadataStatusMessage extends StatusMessage implements Parcelable {
    /**
     * The status of the firmware metadata.
     * It is represented by 3 bits.
     *
     * @see UpdateStatus
     */
    private int status;

    /**
     * Additional information about the firmware metadata.
     * It is represented by 5 bits.
     *
     * @see AdditionalInformation
     */
    private int additionalInformation;

    /**
     * The index of the firmware image in the Firmware Information List state that was checked.
     * It is represented by 8 bits.
     */
    private int updateFirmwareImageIndex;

    /**
     * Default constructor for the FirmwareMetadataStatusMessage class.
     */
    public FirmwareMetadataStatusMessage() {
    }

    /**
     * Constructor for the FirmwareMetadataStatusMessage class.
     * It initializes the status, additionalInformation, and updateFirmwareImageIndex fields from the provided Parcel.
     *
     * @param in The Parcel from which to read the values.
     */
    protected FirmwareMetadataStatusMessage(Parcel in) {
        status = in.readInt();
        additionalInformation = in.readInt();
        updateFirmwareImageIndex = in.readInt();
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     * It is used to create new instances of the FirmwareMetadataStatusMessage class from a Parcel.
     */
    public static final Creator<FirmwareMetadataStatusMessage> CREATOR = new Creator<FirmwareMetadataStatusMessage>() {
        @Override
        public FirmwareMetadataStatusMessage createFromParcel(Parcel in) {
            return new FirmwareMetadataStatusMessage(in);
        }

        @Override
        public FirmwareMetadataStatusMessage[] newArray(int size) {
            return new FirmwareMetadataStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the values of the status, additionalInformation, and updateFirmwareImageIndex fields accordingly.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index] & 0x07;
        this.additionalInformation = (params[index++] >> 3) & 0x1F;
        this.updateFirmwareImageIndex = params[index] & 0xFF;
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0, as there are no special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the object into a Parcel.
     * It writes the values of the status, additionalInformation, and updateFirmwareImageIndex fields into the Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(additionalInformation);
        dest.writeInt(updateFirmwareImageIndex);
    }

    /**
     * Returns the status of the firmware metadata.
     *
     * @return The status of the firmware metadata.
     */
    public int getStatus() {
        return status;
    }

    /**
     * Returns the additional information about the firmware metadata.
     *
     * @return The additional information about the firmware metadata.
     */
    public int getAdditionalInformation() {
        return additionalInformation;
    }

    /**
     * Returns the index of the firmware image in the Firmware Information List state that was checked.
     *
     * @return The index of the firmware image.
     */
    public int getUpdateFirmwareImageIndex() {
        return updateFirmwareImageIndex;
    }
}