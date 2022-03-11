/********************************************************************************************************
 * @file LevelStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/9/3.
 */

public class LevelStatusMessage extends StatusMessage {

    private static final int DATA_LEN_COMPLETE = 5;

    /**
     * The present value of the Generic Level state.
     * 2 bytes
     */
    private int presentLevel;

    /**
     * The target value of the Generic Level state (Optional).
     * 2 bytes
     */
    private int targetLevel;

    private byte remainingTime;

    private boolean isComplete = false;

    public LevelStatusMessage() {
    }

    protected LevelStatusMessage(Parcel in) {
        presentLevel = in.readInt();
        targetLevel = in.readInt();
        remainingTime = in.readByte();
    }

    public static final Creator<LevelStatusMessage> CREATOR = new Creator<LevelStatusMessage>() {
        @Override
        public LevelStatusMessage createFromParcel(Parcel in) {
            return new LevelStatusMessage(in);
        }

        @Override
        public LevelStatusMessage[] newArray(int size) {
            return new LevelStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.presentLevel = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLevel = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
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
        dest.writeInt(presentLevel);
        dest.writeInt(targetLevel);
        dest.writeByte(remainingTime);
    }

    public int getPresentLevel() {
        return presentLevel;
    }

    public int getTargetLevel() {
        return targetLevel;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }
}
