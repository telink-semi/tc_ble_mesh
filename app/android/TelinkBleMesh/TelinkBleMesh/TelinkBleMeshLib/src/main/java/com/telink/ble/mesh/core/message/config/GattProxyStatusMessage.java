/********************************************************************************************************
 * @file GattProxyStatusMessage.java
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
 * The Config GATT Proxy Status is an unacknowledged message used to report the current GATT Proxy state of a node
 * <p>
 * response of {@link GattProxyGetMessage} {@link GattProxySetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_GATT_PROXY_STATUS
 */
public class GattProxyStatusMessage extends StatusMessage implements Parcelable {


    public byte gattProxy;


    public GattProxyStatusMessage() {
    }


    protected GattProxyStatusMessage(Parcel in) {
        gattProxy = in.readByte();
    }

    public static final Creator<GattProxyStatusMessage> CREATOR = new Creator<GattProxyStatusMessage>() {
        @Override
        public GattProxyStatusMessage createFromParcel(Parcel in) {
            return new GattProxyStatusMessage(in);
        }

        @Override
        public GattProxyStatusMessage[] newArray(int size) {
            return new GattProxyStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        gattProxy = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(gattProxy);
    }
}
