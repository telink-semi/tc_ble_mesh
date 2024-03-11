/********************************************************************************************************
 * @file SarReceiverStatusMessage.java
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
 * SarReceiverStatusMessage
 * This class represents a status message for the SAR (Segmentation and Reassembly) receiver.
 * It extends the StatusMessage class and implements the Parcelable interface for easy serialization.
 * The class contains various state variables that represent different aspects of the SAR receiver's status.
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
     * New SAR Acknowledgment Retransmissions Count state
     */
    public int ackRetransCount;

    /**
     * Reserved for Future Use
     */
    public int rfu;

    /**
     * Default constructor for SarReceiverStatusMessage.
     */
    public SarReceiverStatusMessage() {
    }

    /**
     * Constructor for SarReceiverStatusMessage that takes a Parcel as input.
     * Used for deserialization.
     *
     * @param in The Parcel object containing the serialized SarReceiverStatusMessage.
     */
    protected SarReceiverStatusMessage(Parcel in) {
        segmentsThreshold = in.readInt();
        ackDelayIncrement = in.readInt();
        discardTimeout = in.readInt();
        receiverSegmentIntervalStep = in.readInt();
        ackRetransCount = in.readInt();
        rfu = in.readInt();
    }

    /**
     * Creator object for SarReceiverStatusMessage.
     * Used for deserialization.
     */
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

    /**
     * Parses the byte array representation of the SAR receiver status message.
     * Extracts the state variables from the byte array and assigns them to the corresponding fields.
     *
     * @param params The byte array representation of the SAR receiver status message.
     */
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

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     * Always returns 0 in this implementation.
     *
     * @return An integer value representing the bitmask.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flatten this object into a Parcel.
     * Serializes the SarReceiverStatusMessage object into a Parcel.
     *
     * @param dest  The Parcel object to write the serialized data to.
     * @param flags Additional flags about how the object should be written.
     */
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