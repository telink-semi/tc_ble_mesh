/********************************************************************************************************
 * @file DefaultTTLStatusMessage.java
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
 * The Config Default TTL Status is an unacknowledged message used to report the current Default TTL state of a node
 * <p>
 * response of {@link DefaultTTLSetMessage} {@link DefaultTTLGetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_DEFAULT_TTL_STATUS
 */
public class DefaultTTLStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Default TTL
     */
    public byte ttl;


    public DefaultTTLStatusMessage() {
    }


    protected DefaultTTLStatusMessage(Parcel in) {
        ttl = in.readByte();
    }

    public static final Creator<DefaultTTLStatusMessage> CREATOR = new Creator<DefaultTTLStatusMessage>() {
        @Override
        public DefaultTTLStatusMessage createFromParcel(Parcel in) {
            return new DefaultTTLStatusMessage(in);
        }

        @Override
        public DefaultTTLStatusMessage[] newArray(int size) {
            return new DefaultTTLStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        ttl = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(ttl);
    }
}
