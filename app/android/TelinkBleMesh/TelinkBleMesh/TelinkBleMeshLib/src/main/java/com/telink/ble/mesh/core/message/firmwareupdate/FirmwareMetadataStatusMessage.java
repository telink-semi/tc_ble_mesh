/********************************************************************************************************
 * @file FirmwareMetadataStatusMessage.java
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

public class FirmwareMetadataStatusMessage extends StatusMessage implements Parcelable {

    /**
     * @see UpdateStatus
     * 3 bits
     */
    private int status;

    /**
     * 5 bits
     *
     * @see AdditionalInformation
     */
    private int additionalInformation;

    /**
     * Index of the firmware image in the Firmware Information List state that was checked
     * 8 bits
     */
    private int updateFirmwareImageIndex;


    public FirmwareMetadataStatusMessage() {
    }


    protected FirmwareMetadataStatusMessage(Parcel in) {
        status = in.readInt();
        additionalInformation = in.readInt();
        updateFirmwareImageIndex = in.readInt();
    }

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

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index] & 0x07;
        this.additionalInformation = (params[index++] >> 3) & 0x1F;
        this.updateFirmwareImageIndex = params[index] & 0xFF;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(additionalInformation);
        dest.writeInt(updateFirmwareImageIndex);
    }

    public int getStatus() {
        return status;
    }

    public int getAdditionalInformation() {
        return additionalInformation;
    }

    public int getUpdateFirmwareImageIndex() {
        return updateFirmwareImageIndex;
    }
}
