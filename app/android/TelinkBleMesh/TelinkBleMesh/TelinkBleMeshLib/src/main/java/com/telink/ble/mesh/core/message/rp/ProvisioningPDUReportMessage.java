/********************************************************************************************************
 * @file ProvisioningPDUReportMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.Arrays;


/**
 * Created by kee on 2019/8/20.
 */

public class ProvisioningPDUReportMessage extends StatusMessage implements Parcelable {

    private byte inboundPDUNumber;

    private byte[] provisioningPDU;

    public ProvisioningPDUReportMessage() {
    }


    protected ProvisioningPDUReportMessage(Parcel in) {
        inboundPDUNumber = in.readByte();
        provisioningPDU = in.createByteArray();
    }

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

    @Override
    public void parse(byte[] params) {
        this.inboundPDUNumber = params[0];
        if (params.length > 1) {
            int pduLen = params.length - 1;
            provisioningPDU = new byte[pduLen];
            System.arraycopy(params, 1, this.provisioningPDU, 0, pduLen);
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(inboundPDUNumber);
        dest.writeByteArray(provisioningPDU);
    }

    public byte getInboundPDUNumber() {
        return inboundPDUNumber;
    }

    public byte[] getProvisioningPDU() {
        return provisioningPDU;
    }

    @Override
    public String toString() {
        return "ProvisioningPDUReportMessage{" +
                "inboundPDUNumber=" + inboundPDUNumber +
                ", provisioningPDU=" + Arrays.bytesToHexString(provisioningPDU) +
                '}';
    }
}
