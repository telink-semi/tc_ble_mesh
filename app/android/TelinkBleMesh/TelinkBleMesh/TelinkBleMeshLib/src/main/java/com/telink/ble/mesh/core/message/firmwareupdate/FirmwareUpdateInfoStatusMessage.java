/********************************************************************************************************
 * @file FirmwareUpdateInfoStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

    public FirmwareUpdateInfoStatusMessage() {
    }


    protected FirmwareUpdateInfoStatusMessage(Parcel in) {
        listCount = in.readInt();
        firstIndex = in.readInt();
        firmwareInformationList = in.createTypedArrayList(FirmwareInformationEntry.CREATOR);
    }

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

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(listCount);
        dest.writeInt(firstIndex);
        dest.writeTypedList(firmwareInformationList);
    }

    public int getListCount() {
        return listCount;
    }

    public int getFirstIndex() {
        return firstIndex;
    }

    public List<FirmwareInformationEntry> getFirmwareInformationList() {
        return firmwareInformationList;
    }

    public FirmwareInformationEntry getFirstEntry() {
        if (firmwareInformationList == null || firmwareInformationList.size() < (firstIndex + 1)) {
            return null;
        }
        return firmwareInformationList.get(firstIndex);
    }

    public static class FirmwareInformationEntry implements Parcelable {
        /**
         * Length of the Current Firmware ID field
         * 1 byte
         */
        public int currentFirmwareIDLength;

        /**
         * Identifies the firmware image on the node or any subsystem on the node.
         * Variable
         */
        public byte[] currentFirmwareID;

        /**
         * Length of the Update URI field
         * 1 byte
         */
        public int updateURILength;

        /**
         * URI used to retrieve a new firmware image
         * If the value of the Update URI Length field is greater than 0, the Update URI field shall be present.
         */
        public byte[] updateURI;

        public FirmwareInformationEntry() {
        }

        protected FirmwareInformationEntry(Parcel in) {
            currentFirmwareIDLength = in.readInt();
            currentFirmwareID = in.createByteArray();
            updateURILength = in.readInt();
            updateURI = in.createByteArray();
        }

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

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(currentFirmwareIDLength);
            dest.writeByteArray(currentFirmwareID);
            dest.writeInt(updateURILength);
            dest.writeByteArray(updateURI);
        }
    }

    @Override
    public String toString() {
        return "FirmwareUpdateInfoStatusMessage{" +
                "listCount=" + listCount +
                ", firstIndex=" + firstIndex +
                ", firmwareInformationList=" + firmwareInformationList.size() +
                '}';
    }
}
