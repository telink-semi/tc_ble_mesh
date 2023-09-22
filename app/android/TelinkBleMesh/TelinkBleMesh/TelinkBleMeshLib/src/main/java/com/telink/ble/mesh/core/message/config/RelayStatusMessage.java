/********************************************************************************************************
 * @file RelayStatusMessage.java
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
 * The Config Relay Status is an unacknowledged message used to report the current Relay and Relay Retransmit states of a node.
 * <p>
 * response of {@link RelayGetMessage} {@link RelaySetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_RELAY_STATUS
 */
public class RelayStatusMessage extends StatusMessage implements Parcelable {


    /**
     * 0x00 The node support Relay feature that is disabled
     * 0x01 The node supports Relay feature that is enabled
     * 0x02 Relay feature is not supported
     * 8 bits (1 byte)
     */
    public byte relay;

    /**
     * 3 bits
     */
    public byte retransmitCount;

    /**
     * Number of 10-millisecond steps between retransmissions
     * retransmission interval = (Relay Retransmit Interval Steps + 1) * 10
     * 5 bits
     */
    public byte retransmitIntervalSteps;


    /**
     * Default constructor for RelayStatusMessage.
     */
    public RelayStatusMessage() {
    }

    /**
     * Constructor for RelayStatusMessage that initializes the relay status from a Parcel.
     *
     * @param in The Parcel from which to read the subnet bridge state.
     */
    protected RelayStatusMessage(Parcel in) {
        relay = in.readByte();
        retransmitCount = in.readByte();
        retransmitIntervalSteps = in.readByte();
    }

    /**
     * Creator for RelayStatusMessage that is used to create new instances from a Parcel.
     */
    public static final Creator<RelayStatusMessage> CREATOR = new Creator<RelayStatusMessage>() {
        @Override
        public RelayStatusMessage createFromParcel(Parcel in) {
            return new RelayStatusMessage(in);
        }

        @Override
        public RelayStatusMessage[] newArray(int size) {
            return new RelayStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the fields of the RelayStatusMessage object accordingly.
     *
     * @param params the byte array to parse
     */
    @Override
    public void parse(byte[] params) {
        relay = params[0];
        this.retransmitCount = (byte) (params[1] & 0b111);
        this.retransmitIntervalSteps = (byte) ((params[1] & 0xFF) >> 3);
    }

    /**
     * Returns the bitmask describing the contents of the Parcelable.
     *
     * @return The bitmask describing the contents of the Parcelable.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the fields of the RelayStatusMessage object to a Parcel object.
     *
     * @param dest  the Parcel object
     * @param flags additional flags for writing the object
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(relay);
        dest.writeByte(retransmitCount);
        dest.writeByte(retransmitIntervalSteps);
    }
}
