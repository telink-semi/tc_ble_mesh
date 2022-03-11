/********************************************************************************************************
 * @file FDReceiversStatusMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

public class FDReceiversStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Status Code for the requesting message
     * 1 byte
     */
    private int status;

    /**
     * Receivers List Count
     * The number of entries in the Distribution Receivers List state
     * 2 bytes
     */
    private int receiversListCount;

    public FDReceiversStatusMessage() {
    }


    protected FDReceiversStatusMessage(Parcel in) {
        status = in.readInt();
        receiversListCount = in.readInt();
    }

    public static final Creator<FDReceiversStatusMessage> CREATOR = new Creator<FDReceiversStatusMessage>() {
        @Override
        public FDReceiversStatusMessage createFromParcel(Parcel in) {
            return new FDReceiversStatusMessage(in);
        }

        @Override
        public FDReceiversStatusMessage[] newArray(int size) {
            return new FDReceiversStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++] & 0xFF;
        this.receiversListCount = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    public int getStatus() {
        return status;
    }

    public int getReceiversListCount() {
        return receiversListCount;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(receiversListCount);
    }

    @Override
    public String toString() {
        return "FirmwareDistributionReceiversStatus{" +
                "status=" + status +
                ", receiversListCount=" + receiversListCount +
                '}';
    }
}
