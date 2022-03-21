/********************************************************************************************************
 * @file FriendStatusMessage.java
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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * The Config Friend Status is an unacknowledged message used to report the current Friend state of a node
 * <p>
 * response of {@link FriendGetMessage} {@link FriendSetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_FRIEND_STATUS
 */
public class FriendStatusMessage extends StatusMessage implements Parcelable {


    /**
     * Friend state
     */
    public byte friend;


    public FriendStatusMessage() {
    }


    protected FriendStatusMessage(Parcel in) {
        friend = in.readByte();
    }

    public static final Creator<FriendStatusMessage> CREATOR = new Creator<FriendStatusMessage>() {
        @Override
        public FriendStatusMessage createFromParcel(Parcel in) {
            return new FriendStatusMessage(in);
        }

        @Override
        public FriendStatusMessage[] newArray(int size) {
            return new FriendStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        friend = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(friend);
    }
}
