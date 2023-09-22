/********************************************************************************************************
 * @file FirmwareUpdateStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a firmware update status message.
 * It extends the StatusMessage class and implements the Parcelable interface for easy transfer of objects between components.
 */
public class FirmwareUpdateStatusMessage extends StatusMessage implements Parcelable {
    /**
     * The status of the firmware update.
     * It is represented by the 3 lower bits in the first byte of the message.
     */
    private int status;
    /**
     * The phase of the firmware update.
     * It is represented by the 3 higher bits in the first byte of the message.
     */
    private int phase;
    /**
     * The Time To Live (TTL) value to use during firmware image transfer.
     * It is represented by 1 byte in the message.
     */
    private byte updateTtl;
    /**
     * Additional information about the firmware update.
     * It is represented by 5 bits in the message (3 bits reserved for future use).
     */
    private int additionalInfo;
    /**
     * The base value used to compute the timeout of the firmware image transfer.
     * The client timeout is calculated as [12,000 * (Client Timeout Base + 1) + 100 * Transfer TTL] milliseconds.
     * It is represented by 2 bytes in the message.
     */
    private int updateTimeoutBase;
    /**
     * The BLOB identifier for the firmware image.
     * It is represented by 8 bytes in the message.
     */
    private long updateBLOBID;
    /**
     * The index of the firmware image being updated.
     * It is represented by 1 byte in the message.
     */
    private int updateFirmwareImageIndex;
    /**
     * Indicates if the firmware update message is complete or not.
     * If the Update TTL field is present, the Additional Information field, Update Timeout field, BLOB ID field, and Installed Firmware ID field shall be present;
     * otherwise, the Additional Information field, Update Timeout field, BLOB ID field, and Installed Firmware ID field shall not be present.
     */
    private boolean isComplete = false;

    /**
     * Default constructor for the FirmwareUpdateStatusMessage class.
     */
    public FirmwareUpdateStatusMessage() {
    }

    /**
     * Constructor for the FirmwareUpdateStatusMessage class that takes a Parcel as input.
     * It is used for Parcelable implementation.
     *
     * @param in The Parcel object to read the values from.
     */
    protected FirmwareUpdateStatusMessage(Parcel in) {
        status = in.readInt();
        phase = in.readInt();
        updateTtl = in.readByte();
        additionalInfo = in.readInt();
        updateTimeoutBase = in.readInt();
        updateBLOBID = in.readLong();
        updateFirmwareImageIndex = in.readInt();
        isComplete = in.readByte() != 0;
    }

    /**
     * Parcelable creator for the FirmwareUpdateStatusMessage class.
     */
    public static final Creator<FirmwareUpdateStatusMessage> CREATOR = new Creator<FirmwareUpdateStatusMessage>() {
        @Override
        public FirmwareUpdateStatusMessage createFromParcel(Parcel in) {
            return new FirmwareUpdateStatusMessage(in);
        }

        @Override
        public FirmwareUpdateStatusMessage[] newArray(int size) {
            return new FirmwareUpdateStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to populate the fields of the FirmwareUpdateStatusMessage object.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index] & 0x07;
        this.phase = (params[index] & 0xFF) >> 5;
        isComplete = params.length > 1;
        if (!isComplete) return;
        index++;
        this.updateTtl = params[index++];
        this.additionalInfo = (params[index++] & 0x1F);
        this.updateTimeoutBase = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.updateBLOBID = MeshUtils.bytes2Integer(params, index, 8, ByteOrder.LITTLE_ENDIAN);
        index += 8;
        this.updateFirmwareImageIndex = params[index] & 0xFF;
    }

    /**
     * Returns the status of the firmware update.
     *
     * @return The status of the firmware update.
     */
    public int getStatus() {
        return status;
    }

    /**
     * Returns the phase of the firmware update.
     *
     * @return The phase of the firmware update.
     */
    public int getPhase() {
        return phase;
    }

    /**
     * Returns the Time To Live (TTL) value used during firmware image transfer.
     *
     * @return The Time To Live (TTL) value used during firmware image transfer.
     */
    public byte getUpdateTtl() {
        return updateTtl;
    }

    /**
     * Returns the additional information about the firmware update.
     *
     * @return The additional information about the firmware update.
     */
    public int getAdditionalInfo() {
        return additionalInfo;
    }

    /**
     * Returns the base value used to compute the timeout of the firmware image transfer.
     *
     * @return The base value used to compute the timeout of the firmware image transfer.
     */
    public int getUpdateTimeoutBase() {
        return updateTimeoutBase;
    }

    /**
     * Returns the BLOB identifier for the firmware image.
     *
     * @return The BLOB identifier for the firmware image.
     */
    public long getUpdateBLOBID() {
        return updateBLOBID;
    }

    /**
     * Returns the index of the firmware image being updated.
     *
     * @return The index of the firmware image being updated.
     */
    public int getUpdateFirmwareImageIndex() {
        return updateFirmwareImageIndex;
    }

    /**
     * Returns whether the firmware update message is complete or not.
     *
     * @return True if the firmware update message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }

    /**
     * Writes the values of the FirmwareUpdateStatusMessage object to a Parcel.
     * It is used for Parcelable implementation.
     *
     * @param dest  The Parcel object to write the values to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(phase);
        dest.writeByte(updateTtl);
        dest.writeInt(additionalInfo);
        dest.writeInt(updateTimeoutBase);
        dest.writeLong(updateBLOBID);
        dest.writeInt(updateFirmwareImageIndex);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Describes the contents of the FirmwareUpdateStatusMessage object.
     *
     * @return An integer representing the contents of the object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Returns a string representation of the FirmwareUpdateStatusMessage object.
     *
     * @return A string representation of the FirmwareUpdateStatusMessage object.
     */
    @Override
    public String toString() {
        return "FirmwareUpdateStatusMessage{" +
                "status=" + status +
                ", phase=" + phase +
                ", updateTtl=" + updateTtl +
                ", additionalInfo=" + additionalInfo +
                ", updateTimeoutBase=" + updateTimeoutBase +
                ", updateBLOBID=0x" + Long.toHexString(updateBLOBID) +
                ", updateFirmwareImageIndex=" + updateFirmwareImageIndex +
                ", isComplete=" + isComplete +
                '}';
    }
}