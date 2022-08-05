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
public class SarReceiverStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 5 bits
     * New SAR Segments Threshold state
     */
    public int segmentsThreshold;

    /**
     * 3 bits
     * New SAR Acknowledgment Delay Increment state
     */
    public int ackDelayIncrement;

    /**
     * 4 bits
     * New SAR Discard Timeout state
     */
    public int discardTimeout;

    /**
     * 4 bits
     * New SAR Receiver Segment Interval Step state
     */
    public int receiverSegmentIntervalStep;

    /**
     * 2 bits
     *  New SAR Acknowledgment Retransmissions Count state
     */
    public int ackRetransCount;

    /**
     * Reserved for Future Use
     */
    public int rfu;
    public SarReceiverStatusMessage() {
    }


    protected SarReceiverStatusMessage(Parcel in) {
        segmentsThreshold = in.readInt();
        ackDelayIncrement = in.readInt();
        discardTimeout = in.readInt();
        receiverSegmentIntervalStep = in.readInt();
        ackRetransCount = in.readInt();
        rfu = in.readInt();
    }

    public static final Creator<SarReceiverStatusMessage> CREATOR = new Creator<SarReceiverStatusMessage>() {
        @Override
        public SarReceiverStatusMessage createFromParcel(Parcel in) {
            return new SarReceiverStatusMessage(in);
        }

        @Override
        public SarReceiverStatusMessage[] newArray(int size) {
            return new SarReceiverStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        segmentsThreshold = params[index] & 0xFF;
        ackDelayIncrement = (params[index++] & 0xFF) >> 5;

        discardTimeout = params[index] & 0xFF;
        receiverSegmentIntervalStep = (params[index++] & 0xFF) >> 4;

        ackRetransCount = params[index] & 0xFF;
        rfu = (params[index++] & 0xFF) >> 2;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(segmentsThreshold);
        dest.writeInt(ackDelayIncrement);
        dest.writeInt(discardTimeout);
        dest.writeInt(receiverSegmentIntervalStep);
        dest.writeInt(ackRetransCount);
        dest.writeInt(rfu);
    }
}
