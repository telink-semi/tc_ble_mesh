/********************************************************************************************************
 * @file ModelAppStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
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


    public RelayStatusMessage() {
    }

    protected RelayStatusMessage(Parcel in) {
        relay = in.readByte();
        retransmitCount = in.readByte();
        retransmitIntervalSteps = in.readByte();
    }

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

    @Override
    public void parse(byte[] params) {
        relay = params[0];
        this.retransmitCount = (byte) (params[1] & 0b111);
        this.retransmitIntervalSteps = (byte) ((params[1] & 0xFF) >> 3);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(relay);
        dest.writeByte(retransmitCount);
        dest.writeByte(retransmitIntervalSteps);
    }
}
