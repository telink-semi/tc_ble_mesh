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
 * The DefaultTTLStatusMessage class represents an unacknowledged message used to report the current Default TTL state of a node.
 * It is the response to DefaultTTLSetMessage and DefaultTTLGetMessage.
 * response of {@link DefaultTTLSetMessage} {@link DefaultTTLGetMessage}
 * <p>
 * This message has an opcode of CFG_DEFAULT_TTL_STATUS.
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_DEFAULT_TTL_STATUS
 */
public class DefaultTTLStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Default TTL value
     */
    public byte ttl;

    /**
     * Constructs a new DefaultTTLStatusMessage object.
     */
    public DefaultTTLStatusMessage() {
    }

    /**
     * Constructs a new DefaultTTLStatusMessage object from a Parcel.
     *
     * @param in the Parcel object containing the DefaultTTLStatusMessage data
     */
    protected DefaultTTLStatusMessage(Parcel in) {
        ttl = in.readByte();
    }

    /**
     * A Creator object that generates instances of DefaultTTLStatusMessage from a Parcel.
     */
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

    /**
     * Parses the parameters of the DefaultTTLStatusMessage from a byte array.
     *
     * @param params the byte array containing the parameters of the DefaultTTLStatusMessage
     */
    @Override
    public void parse(byte[] params) {
        ttl = params[0];
    }

    /**
     * Describes the contents of the DefaultTTLStatusMessage object.
     *
     * @return an integer representing the contents of the DefaultTTLStatusMessage object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the DefaultTTLStatusMessage object to a Parcel.
     *
     * @param dest  the Parcel object to write the DefaultTTLStatusMessage object to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(ttl);
    }
}