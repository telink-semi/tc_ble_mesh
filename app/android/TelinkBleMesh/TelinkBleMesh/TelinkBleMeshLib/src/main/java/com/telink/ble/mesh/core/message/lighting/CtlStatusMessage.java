/********************************************************************************************************
 * @file CtlStatusMessage.java
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
package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class CtlStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 9;

    private int presentLightness;

    private int presentTemperature;

    /**
     * The target value of the Lightness/Temperature state (optional).
     */
    private int targetLightness;

    private int targetTemperature;


    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    public CtlStatusMessage() {
    }

    protected CtlStatusMessage(Parcel in) {
        presentLightness = in.readInt();
        presentTemperature = in.readInt();
        targetLightness = in.readInt();
        targetTemperature = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<CtlStatusMessage> CREATOR = new Creator<CtlStatusMessage>() {
        @Override
        public CtlStatusMessage createFromParcel(Parcel in) {
            return new CtlStatusMessage(in);
        }

        @Override
        public CtlStatusMessage[] newArray(int size) {
            return new CtlStatusMessage[size];
        }
    };


    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.presentTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.targetTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.remainingTime = params[index];
        }
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentLightness);
        dest.writeInt(presentTemperature);
        dest.writeInt(targetLightness);
        dest.writeInt(targetTemperature);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }


    public int getPresentLightness() {
        return presentLightness;
    }

    public int getPresentTemperature() {
        return presentTemperature;
    }

    public int getTargetLightness() {
        return targetLightness;
    }

    public int getTargetTemperature() {
        return targetTemperature;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }

    @Override
    public String toString() {
        return "CtlStatusMessage{" +
                "presentLightness=" + presentLightness +
                ", presentTemperature=" + presentTemperature +
                ", targetLightness=" + targetLightness +
                ", targetTemperature=" + targetTemperature +
                ", remainingTime=" + remainingTime +
                ", isComplete=" + isComplete +
                '}';
    }
}
