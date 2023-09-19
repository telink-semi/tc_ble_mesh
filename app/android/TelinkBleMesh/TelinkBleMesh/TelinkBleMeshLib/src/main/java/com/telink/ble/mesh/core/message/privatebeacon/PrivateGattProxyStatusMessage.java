/********************************************************************************************************
 * @file PrivateGattProxyStatusMessage.java
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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * A PRIVATE_GATT_PROXY_STATUS message is an unacknowledged message used to
 * report the current Private GATT Proxy state of a node
 */

public class PrivateGattProxyStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * New private gatt proxy state
     */
    public byte privateGattProxy;

    public PrivateGattProxyStatusMessage() {
    }


    protected PrivateGattProxyStatusMessage(Parcel in) {
        privateGattProxy = in.readByte();
    }

    public static final Creator<PrivateGattProxyStatusMessage> CREATOR = new Creator<PrivateGattProxyStatusMessage>() {
        @Override
        public PrivateGattProxyStatusMessage createFromParcel(Parcel in) {
            return new PrivateGattProxyStatusMessage(in);
        }

        @Override
        public PrivateGattProxyStatusMessage[] newArray(int size) {
            return new PrivateGattProxyStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        privateGattProxy = params[0];
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateGattProxy);
    }
}
