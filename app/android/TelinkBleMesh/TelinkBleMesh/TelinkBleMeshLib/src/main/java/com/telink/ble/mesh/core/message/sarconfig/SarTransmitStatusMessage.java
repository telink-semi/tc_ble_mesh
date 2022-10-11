/********************************************************************************************************
 * @file CompositionDataStatusMessage.java
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
 * A LARGE_COMPOSITION_DATA_STATUS message is an unacknowledged message used to report a
 * portion of a page of the Composition Data
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

    public SarTransmitStatusMessage() {
    }


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

    @Override
    public int describeContents() {
        return 0;
    }

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
