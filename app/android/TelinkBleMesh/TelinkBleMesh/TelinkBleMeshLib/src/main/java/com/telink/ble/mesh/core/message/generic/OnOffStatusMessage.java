/********************************************************************************************************
 * @file OnOffStatusMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/8/20.
 */

public class OnOffStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 3;

    private byte presentOnOff;

    /**
     * The target value of the Generic OnOff state (optional).
     */
    private byte targetOnOff;

    private byte remainingTime;

    private boolean isComplete = false;

    public OnOffStatusMessage() {
    }


    protected OnOffStatusMessage(Parcel in) {
        presentOnOff = in.readByte();
        targetOnOff = in.readByte();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<OnOffStatusMessage> CREATOR = new Creator<OnOffStatusMessage>() {
        @Override
        public OnOffStatusMessage createFromParcel(Parcel in) {
            return new OnOffStatusMessage(in);
        }

        @Override
        public OnOffStatusMessage[] newArray(int size) {
            return new OnOffStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.presentOnOff = params[0];
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetOnOff = params[1];
            this.remainingTime = params[2];
        }
    }

    public byte getPresentOnOff() {
        return presentOnOff;
    }

    public byte getTargetOnOff() {
        return targetOnOff;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(presentOnOff);
        dest.writeByte(targetOnOff);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}
