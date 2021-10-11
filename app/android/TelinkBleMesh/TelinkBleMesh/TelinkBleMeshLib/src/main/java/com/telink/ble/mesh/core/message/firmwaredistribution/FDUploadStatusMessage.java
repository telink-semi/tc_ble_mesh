/********************************************************************************************************
 * @file BlobTransferStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.firmwaredistribution;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.Arrays;

public class FDUploadStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Status Code for the requesting message
     * 1 byte
     */
    private int status;

    /**
     * Upload Phase
     * Phase of the firmware image upload to a Firmware Distribution Server
     * 1 byte
     */
    public int uploadPhase;

    /**
     * Upload Progress
     * A percentage indicating the progress of the firmware image upload (Optional)
     * 0x00 -> 0x64
     * 0 -> 100
     * 7 bits
     */
    public int uploadProgress;


    /**
     * Upload Progress
     * A bit indicating whether the Upload is done in-band or out-of- band
     * 1 bit
     * 0x0	In-band	The firmware upload is executed in-band.
     * 0x1	Out-of-band 	The firmware upload is executed out-of-band.
     */
    public int uploadType;

    /**
     * Upload Firmware ID
     * The Firmware ID identifying the firmware image being uploaded
     * Variable
     */
    public byte[] uploadFirmwareID;

    public FDUploadStatusMessage() {
    }


    protected FDUploadStatusMessage(Parcel in) {
        status = in.readInt();
        uploadPhase = in.readInt();
        uploadProgress = in.readInt();
        uploadFirmwareID = in.createByteArray();
    }

    public static final Creator<FDUploadStatusMessage> CREATOR = new Creator<FDUploadStatusMessage>() {
        @Override
        public FDUploadStatusMessage createFromParcel(Parcel in) {
            return new FDUploadStatusMessage(in);
        }

        @Override
        public FDUploadStatusMessage[] newArray(int size) {
            return new FDUploadStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++] & 0xFF;
        this.uploadPhase = params[index++] & 0xFF;
        if (params.length == 2) return;
        this.uploadProgress = (params[index] & 0x7F) >> 1;
        this.uploadType = params[index++] & 0x01;
        this.uploadFirmwareID = new byte[params.length - index];
        System.arraycopy(params, index, this.uploadFirmwareID, 0, this.uploadFirmwareID.length);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(uploadPhase);
        dest.writeInt(uploadProgress);
        dest.writeByteArray(uploadFirmwareID);
    }

    @Override
    public String toString() {
        return "FDUploadStatusMessage{" +
                "status=" + status +
                ", uploadPhase=" + uploadPhase +
                ", uploadProgress=" + uploadProgress +
                ", uploadType=" + uploadType +
                ", uploadFirmwareID=" + Arrays.bytesToHexString(uploadFirmwareID) +
                '}';
    }
}
