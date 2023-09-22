/********************************************************************************************************
 * @file FirmwareUpdateInfoStatusMessage.java
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

import java.util.ArrayList;
import java.util.List;

/**
 * This class represents a status message for firmware update information.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer.
 * The class contains information about the number of entries in the firmware information list state of the server,
 * the index of the first requested entry, and a list of firmware information entries.
 * The class provides methods to parse the byte array parameters, retrieve the list count, first index, and firmware information list,
 * and get the first entry from the firmware information list.
 */

public class FirmwareUpdateInfoStatusMessage extends StatusMessage implements Parcelable {

    /**
     * The number of entries in the Firmware Information List state of the server
     * 1 byte
     */
    private int listCount;

    /**
     * Index of the first requested entry from the Firmware Information List state
     * 1 byte
     */
    private int firstIndex;

    /**
     * List of entries
     */
    private List<FirmwareInformationEntry> firmwareInformationList;


    /**
     * Default constructor
     */
    public FirmwareUpdateInfoStatusMessage() {
    }

    /**
     * Constructor for Parcelable implementation
     *
     * @param in The Parcel object to read the values from
     */
    protected FirmwareUpdateInfoStatusMessage(Parcel in) {
        listCount = in.readInt();
        firstIndex = in.readInt();
        firmwareInformationList = in.createTypedArrayList(FirmwareInformationEntry.CREATOR);
    }

    /**
     * Creator for Parcelable implementation
     */
    public static final Creator<FirmwareUpdateInfoStatusMessage> CREATOR = new Creator<FirmwareUpdateInfoStatusMessage>() {
        @Override
        public FirmwareUpdateInfoStatusMessage createFromParcel(Parcel in) {
            return new FirmwareUpdateInfoStatusMessage(in);
        }

        @Override
        public FirmwareUpdateInfoStatusMessage[] newArray(int size) {
            return new FirmwareUpdateInfoStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the values of the class fields accordingly.
     *
     * @param params The byte array parameters to parse
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.listCount = params[index++] & 0xFF;
        this.firstIndex = params[index++] & 0xFF;
        firmwareInformationList = new ArrayList<>();
        FirmwareInformationEntry informationEntry;
        int firmwareIdLength;
        byte[] currentFirmwareID;
        int uriLen;
        byte[] updateURI;
        for (int i = 0; i < this.listCount; i++) {
            firmwareIdLength = params[index++] & 0xFF;
            if (firmwareIdLength > 0) {
                currentFirmwareID = new byte[firmwareIdLength];
                System.arraycopy(params, index, currentFirmwareID, 0, firmwareIdLength);
                index += firmwareIdLength;
            } else {
                currentFirmwareID = null;
            }
            uriLen = params[index++];
            if (uriLen > 0) {
                updateURI = new byte[uriLen];
                System.arraycopy(params, index, updateURI, 0, uriLen);
                index += uriLen;
            } else {
                updateURI = null;
            }
            informationEntry = new FirmwareInformationEntry();
            informationEntry.currentFirmwareIDLength = firmwareIdLength;
            informationEntry.currentFirmwareID = currentFirmwareID;
            informationEntry.updateURILength = uriLen;
            informationEntry.updateURI = updateURI;
            firmwareInformationList.add(informationEntry);
        }
    }

    /**
     * Returns the description of the Parcelable object
     *
     * @return The description of the Parcelable object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the values of the class fields to the Parcel object
     *
     * @param dest  The Parcel object to write the values to
     * @param flags Additional flags for the operation
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(listCount);
        dest.writeInt(firstIndex);
        dest.writeTypedList(firmwareInformationList);
    }

    /**
     * Returns the number of entries in the Firmware Information List state of the server
     *
     * @return The number of entries in the Firmware Information List state of the server
     */
    public int getListCount() {
        return listCount;
    }

    /**
     * Returns the index of the first requested entry from the Firmware Information List state
     *
     * @return The index of the first requested entry from the Firmware Information List state
     */
    public int getFirstIndex() {
        return firstIndex;
    }

    /**
     * Returns the list of firmware information entries
     *
     * @return The list of firmware information entries
     */
    public List<FirmwareInformationEntry> getFirmwareInformationList() {
        return firmwareInformationList;
    }

    /**
     * Returns the first entry from the firmware information list
     *
     * @return The first entry from the firmware information list, or null if the list is empty or the first index is out of bounds
     */
    public FirmwareInformationEntry getFirstEntry() {
        if (firmwareInformationList == null || firmwareInformationList.size() < (firstIndex + 1)) {
            return null;
        }
        return firmwareInformationList.get(firstIndex);
    }

    /**
     * This class represents an entry in the firmware information list.
     * It implements the Parcelable interface for easy data transfer.
     * The class contains information about the length of the current firmware ID field,
     * the current firmware ID, the length of the update URI field, and the update URI.
     */
    public static class FirmwareInformationEntry implements Parcelable {

        public int currentFirmwareIDLength; // Length of the Current Firmware ID field
        public byte[] currentFirmwareID; // Identifies the firmware image on the node or any subsystem on the node
        public int updateURILength; // Length of the Update URI field
        public byte[] updateURI; // URI used to retrieve a new firmware image

        /**
         * Default constructor
         */
        public FirmwareInformationEntry() {
        }

        /**
         * Constructor for Parcelable implementation
         *
         * @param in The Parcel object to read the values from
         */
        protected FirmwareInformationEntry(Parcel in) {
            currentFirmwareIDLength = in.readInt();
            currentFirmwareID = in.createByteArray();
            updateURILength = in.readInt();
            updateURI = in.createByteArray();
        }

        /**
         * Creator for Parcelable implementation
         */
        public static final Creator<FirmwareInformationEntry> CREATOR = new Creator<FirmwareInformationEntry>() {
            @Override
            public FirmwareInformationEntry createFromParcel(Parcel in) {
                return new FirmwareInformationEntry(in);
            }

            @Override
            public FirmwareInformationEntry[] newArray(int size) {
                return new FirmwareInformationEntry[size];
            }
        };

        /**
         * Returns the description of the Parcelable object
         *
         * @return The description of the Parcelable object
         */
        @Override
        public int describeContents() {
            return 0;
        }

        /**
         * Writes the values of the class fields to the Parcel object
         *
         * @param dest  The Parcel object to write the values to
         * @param flags Additional flags for the operation
         */
        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(currentFirmwareIDLength);
            dest.writeByteArray(currentFirmwareID);
            dest.writeInt(updateURILength);
            dest.writeByteArray(updateURI);
        }
    }

    /**
     * Returns a string representation of the FirmwareUpdateInfoStatusMessage object
     *
     * @return string representation
     */

    @Override
    public String toString() {
        return "FirmwareUpdateInfoStatusMessage{" +
                "listCount=" + listCount +
                ", firstIndex=" + firstIndex +
                ", firmwareInformationList=" + firmwareInformationList.size() +
                '}';
    }
}