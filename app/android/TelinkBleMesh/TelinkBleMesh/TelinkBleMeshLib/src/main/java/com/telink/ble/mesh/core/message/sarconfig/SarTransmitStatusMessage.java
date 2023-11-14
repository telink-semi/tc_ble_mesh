/********************************************************************************************************
 * @file SarTransmitStatusMessage.java
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
package com.telink.ble.mesh.core.message.sarconfig;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * A SAR Transmit status message is an unacknowledged message
 * This class represents a SAR Transmit status message, which is an unacknowledged message.
 * It extends the StatusMessage class and implements the Parcelable interface for easy data transfer.
 */
public class SarTransmitStatusMessage extends StatusMessage implements Parcelable {

    /**
     * New SAR Segment Interval Step state
     */
    public int segmentIntervalStep;

    /**
     * New SAR Unicast Retransmissions Count state
     */
    public int unicastRetransCnt;

    /**
     * New SAR Unicast Retransmissions Without Progress Count state
     */
    public int unicastRetransWithoutProgCnt;

    /**
     * New SAR Unicast Retransmissions Interval Step state
     */
    public int unicastRetransIntervalStep;

    /**
     * New SAR Unicast Retransmissions Interval Increment state
     */
    public int unicastRetransIntervalIncrement;

    /**
     * New SAR Multicast Retransmissions Count state
     */
    public int multicastRetransCnt;

    /**
     * New SAR Multicast Retransmissions Interval state
     */
    public int multicastRetransInterval;

    /**
     * Reserved for Future Use
     */
    public int rfu;

    /**
     * Default constructor for the SarTransmitStatusMessage class.
     */
    public SarTransmitStatusMessage() {
    }

    /**
     * Constructor for the SarTransmitStatusMessage class that initializes its fields from a Parcel.
     *
     * @param in The Parcel object to read the fields from.
     */
    protected SarTransmitStatusMessage(Parcel in) {
        segmentIntervalStep = in.readInt();
        unicastRetransCnt = in.readInt();
        unicastRetransWithoutProgCnt = in.readInt();
        unicastRetransIntervalStep = in.readInt();
        unicastRetransIntervalIncrement = in.readInt();
        multicastRetransCnt = in.readInt();
        multicastRetransInterval = in.readInt();
        rfu = in.readInt();
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface for the SarTransmitStatusMessage class.
     */
    public static final Creator<SarTransmitStatusMessage> CREATOR = new Creator<SarTransmitStatusMessage>() {
        @Override
        public SarTransmitStatusMessage createFromParcel(Parcel in) {
            return new SarTransmitStatusMessage(in);
        }

        @Override
        public SarTransmitStatusMessage[] newArray(int size) {
            return new SarTransmitStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets the fields of the SarTransmitStatusMessage object accordingly.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        segmentIntervalStep = params[index] & 0x0F;
        unicastRetransCnt = (params[index++] & 0xF0) >> 4;
        unicastRetransWithoutProgCnt = params[index] & 0x0F;
        unicastRetransIntervalStep = (params[index++] & 0xF0) >> 4;
        unicastRetransIntervalIncrement = params[index] & 0x0F;
        multicastRetransCnt = (params[index++] & 0xF0) >> 4;
        multicastRetransInterval = params[index] & 0x0F;
        multicastRetransCnt = (params[index] & 0xF0) >> 4;
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the SarTransmitStatusMessage object into a Parcel.
     *
     * @param dest  The Parcel object to write the fields to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(segmentIntervalStep);
        dest.writeInt(unicastRetransCnt);
        dest.writeInt(unicastRetransWithoutProgCnt);
        dest.writeInt(unicastRetransIntervalStep);
        dest.writeInt(unicastRetransIntervalIncrement);
        dest.writeInt(multicastRetransCnt);
        dest.writeInt(multicastRetransInterval);
        dest.writeInt(rfu);
    }
}

