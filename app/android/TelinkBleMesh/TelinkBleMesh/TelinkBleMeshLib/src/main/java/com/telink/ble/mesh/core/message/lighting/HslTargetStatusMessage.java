/********************************************************************************************************
 * @file HslTargetStatusMessage.java
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
 * There is no target info in status message
 * Created by kee on 2019/8/20.
 */
public class HslTargetStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 7;

    private int targetLightness;

    private int targetHue;

    private int targetSaturation;

    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    public HslTargetStatusMessage() {
    }

    protected HslTargetStatusMessage(Parcel in) {
        targetLightness = in.readInt();
        targetHue = in.readInt();
        targetSaturation = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<HslTargetStatusMessage> CREATOR = new Creator<HslTargetStatusMessage>() {
        @Override
        public HslTargetStatusMessage createFromParcel(Parcel in) {
            return new HslTargetStatusMessage(in);
        }

        @Override
        public HslTargetStatusMessage[] newArray(int size) {
            return new HslTargetStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.targetLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.targetHue = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.targetSaturation = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.remainingTime = params[index];
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(targetLightness);
        dest.writeInt(targetHue);
        dest.writeInt(targetSaturation);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    public int getTargetLightness() {
        return targetLightness;
    }

    public int getTargetHue() {
        return targetHue;
    }

    public int getTargetSaturation() {
        return targetSaturation;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }
}
