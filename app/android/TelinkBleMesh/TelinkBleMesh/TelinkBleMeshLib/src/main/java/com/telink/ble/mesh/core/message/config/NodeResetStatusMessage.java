/********************************************************************************************************
 * @file NodeResetStatusMessage.java
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
 * The Config Node Reset Status is an unacknowledged message used to acknowledge that an element has received a Config Node Reset message.
 */
public class NodeResetStatusMessage extends StatusMessage implements Parcelable {

    public NodeResetStatusMessage() {
    }

    protected NodeResetStatusMessage(Parcel in) {

    }

    public static final Creator<NodeResetStatusMessage> CREATOR = new Creator<NodeResetStatusMessage>() {
        @Override
        public NodeResetStatusMessage createFromParcel(Parcel in) {
            return new NodeResetStatusMessage(in);
        }

        @Override
        public NodeResetStatusMessage[] newArray(int size) {
            return new NodeResetStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {

    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
    }
}
