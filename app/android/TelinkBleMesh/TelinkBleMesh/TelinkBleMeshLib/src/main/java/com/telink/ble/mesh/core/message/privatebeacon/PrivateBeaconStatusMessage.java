/********************************************************************************************************
 * @file PrivateBeaconStatusMessage.java
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
package com.telink.ble.mesh.core.message.privatebeacon;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * A PRIVATE_BEACON_STATUS message is an unacknowledged message used to report the current Private Beacon state and Random Update Interval Steps state of a node.
 * This message is a response to PRIVATE_BEACON_GET message or a PRIVATE_BEACON_SET message.
 */

public class PrivateBeaconStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * New Private Beacon state
     */
    public byte privateBeacon;

    /**
     * 2 bytes
     * New Random Update Interval Steps state (optional)
     */
    public int randomUpdateIntervalSteps;

    public boolean isComplete = false;


    public PrivateBeaconStatusMessage() {
    }


    protected PrivateBeaconStatusMessage(Parcel in) {
        privateBeacon = in.readByte();
        randomUpdateIntervalSteps = in.readInt();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<PrivateBeaconStatusMessage> CREATOR = new Creator<PrivateBeaconStatusMessage>() {
        @Override
        public PrivateBeaconStatusMessage createFromParcel(Parcel in) {
            return new PrivateBeaconStatusMessage(in);
        }

        @Override
        public PrivateBeaconStatusMessage[] newArray(int size) {
            return new PrivateBeaconStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        privateBeacon = params[index++];
        if (params.length >= 3) {
            isComplete = true;
            randomUpdateIntervalSteps = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        }
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateBeacon);
        dest.writeInt(randomUpdateIntervalSteps);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}
