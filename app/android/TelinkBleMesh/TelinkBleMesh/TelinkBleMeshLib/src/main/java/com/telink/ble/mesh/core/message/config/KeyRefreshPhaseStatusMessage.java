/********************************************************************************************************
 * @file KeyRefreshPhaseStatusMessage.java
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
package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config Key Refresh Phase Status is an unacknowledged message used to report the current Key Refresh Phase state of the identified network key
 * <p>
 * response of {@link KeyRefreshPhaseGetMessage} {@link KeyRefreshPhaseSetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_KEY_REFRESH_PHASE_STATUS
 */
public class KeyRefreshPhaseStatusMessage extends StatusMessage implements Parcelable {


    public byte status;

    public int netKeyIndex;

    public byte phase;

    /**
     * Constructs a new KeyRefreshPhaseStatusMessage object.
     */
    public KeyRefreshPhaseStatusMessage() {
    }


    /**
     * Constructs a new KeyRefreshPhaseStatusMessage object from a Parcel.
     *
     * @param in the Parcel object containing the KeyRefreshPhaseStatusMessage data
     */
    protected KeyRefreshPhaseStatusMessage(Parcel in) {
        status = in.readByte();
        netKeyIndex = in.readInt();
        phase = in.readByte();
    }

    /**
     * A Creator object that generates instances of KeyRefreshPhaseStatusMessage from a Parcel.
     */
    public static final Creator<KeyRefreshPhaseStatusMessage> CREATOR = new Creator<KeyRefreshPhaseStatusMessage>() {
        @Override
        public KeyRefreshPhaseStatusMessage createFromParcel(Parcel in) {
            return new KeyRefreshPhaseStatusMessage(in);
        }

        @Override
        public KeyRefreshPhaseStatusMessage[] newArray(int size) {
            return new KeyRefreshPhaseStatusMessage[size];
        }
    };

    /**
     * Parses the parameters of the KeyRefreshPhaseStatusMessage from a byte array.
     *
     * @param params the byte array containing the parameters of the KeyRefreshPhaseStatusMessage
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++];
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        phase = params[index];
    }

    /**
     * Describes the contents of the KeyRefreshPhaseStatusMessage object.
     *
     * @return an integer representing the contents of the KeyRefreshPhaseStatusMessage object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the KeyRefreshPhaseStatusMessage object to a Parcel.
     *
     * @param parcel the Parcel object to write the KeyRefreshPhaseStatusMessage object to
     * @param i      additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeByte(status);
        parcel.writeInt(netKeyIndex);
        parcel.writeByte(phase);
    }
}
