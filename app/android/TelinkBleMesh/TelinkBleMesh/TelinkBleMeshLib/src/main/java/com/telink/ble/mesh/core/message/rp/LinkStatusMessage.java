/********************************************************************************************************
 * @file LinkStatusMessage.java
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

/**
 * Created by kee on 2019/8/20.
 */

public class LinkStatusMessage extends StatusMessage implements Parcelable {

    private byte status;

    private byte rpState;


    public LinkStatusMessage() {
    }

    protected LinkStatusMessage(Parcel in) {
        status = in.readByte();
        rpState = in.readByte();
    }

    public static final Creator<LinkStatusMessage> CREATOR = new Creator<LinkStatusMessage>() {
        @Override
        public LinkStatusMessage createFromParcel(Parcel in) {
            return new LinkStatusMessage(in);
        }

        @Override
        public LinkStatusMessage[] newArray(int size) {
            return new LinkStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.rpState = params[index++];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeByte(rpState);
    }

    public byte getRpState() {
        return rpState;
    }

    public byte getStatus() {
        return status;
    }


    @Override
    public String toString() {
        return "LinkStatusMessage{" +
                "status=" + status +
                ", rpState=" + rpState +
                '}';
    }
}
