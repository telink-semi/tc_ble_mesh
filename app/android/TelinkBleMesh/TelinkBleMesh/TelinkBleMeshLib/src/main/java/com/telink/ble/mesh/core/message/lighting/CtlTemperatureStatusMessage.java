/********************************************************************************************************
 * @file CtlTemperatureStatusMessage.java
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

public class CtlTemperatureStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 9;


    private int presentTemperature;

    private int presentDeltaUV;

    private int targetTemperature;

    private int targetDeltaUV;


    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    public CtlTemperatureStatusMessage() {
    }

    protected CtlTemperatureStatusMessage(Parcel in) {
        presentTemperature = in.readInt();
        presentDeltaUV = in.readInt();
        targetTemperature = in.readInt();
        targetDeltaUV = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<CtlTemperatureStatusMessage> CREATOR = new Creator<CtlTemperatureStatusMessage>() {
        @Override
        public CtlTemperatureStatusMessage createFromParcel(Parcel in) {
            return new CtlTemperatureStatusMessage(in);
        }

        @Override
        public CtlTemperatureStatusMessage[] newArray(int size) {
            return new CtlTemperatureStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.presentDeltaUV = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetTemperature = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            this.targetDeltaUV = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
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
        dest.writeInt(presentTemperature);
        dest.writeInt(presentDeltaUV);
        dest.writeInt(targetTemperature);
        dest.writeInt(targetDeltaUV);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    public int getPresentTemperature() {
        return presentTemperature;
    }

    public int getPresentDeltaUV() {
        return presentDeltaUV;
    }

    public int getTargetTemperature() {
        return targetTemperature;
    }

    public int getTargetDeltaUV() {
        return targetDeltaUV;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }
}
