/********************************************************************************************************
 * @file NotificationMessage.java
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
package com.telink.ble.mesh.core.message;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * NotificationMessage is used to notify mesh status changed
 * <p>
 * Created by kee on 2019/9/3.
 */
public class NotificationMessage implements Parcelable {
    private int src;
    private int dst;
    private int opcode;

    // params raw data
    private byte[] params;

    /**
     * parsed message by params, if opcode is registered in {@link MeshStatus.Container}
     * otherwise statusMessage will be null
     */
    private StatusMessage statusMessage;

    public NotificationMessage(int src, int dst, int opcode, byte[] params) {
        this.src = src;
        this.dst = dst;
        this.opcode = opcode;
        this.params = params;
        parseStatusMessage();
    }

    private void parseStatusMessage() {
        this.statusMessage = StatusMessage.createByAccessMessage(opcode, params);
    }


    protected NotificationMessage(Parcel in) {
        src = in.readInt();
        dst = in.readInt();
        opcode = in.readInt();
        params = in.createByteArray();
        statusMessage = in.readParcelable(StatusMessage.class.getClassLoader());
    }

    public static final Creator<NotificationMessage> CREATOR = new Creator<NotificationMessage>() {
        @Override
        public NotificationMessage createFromParcel(Parcel in) {
            return new NotificationMessage(in);
        }

        @Override
        public NotificationMessage[] newArray(int size) {
            return new NotificationMessage[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(src);
        dest.writeInt(dst);
        dest.writeInt(opcode);
        dest.writeByteArray(params);
        dest.writeParcelable(statusMessage, flags);
    }

    public int getSrc() {
        return src;
    }

    public void setSrc(int src) {
        this.src = src;
    }

    public int getDst() {
        return dst;
    }

    public void setDst(int dst) {
        this.dst = dst;
    }

    public int getOpcode() {
        return opcode;
    }

    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    public byte[] getParams() {
        return params;
    }

    public void setParams(byte[] params) {
        this.params = params;
    }

    public StatusMessage getStatusMessage() {
        return statusMessage;
    }

    public void setStatusMessage(StatusMessage statusMessage) {
        this.statusMessage = statusMessage;
    }
}
