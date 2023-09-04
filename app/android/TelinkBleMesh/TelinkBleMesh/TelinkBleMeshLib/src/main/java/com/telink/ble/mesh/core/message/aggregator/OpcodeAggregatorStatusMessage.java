/********************************************************************************************************
 * @file OpcodeAggregatorStatusMessage.java
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
package com.telink.ble.mesh.core.message.aggregator;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * An OPCODES_AGGREGATOR_STATUS message is an unacknowledged message used to report status for the most recent operation
 * and response messages as a result of processing the Items field in an OPCODES_AGGREGATOR_SEQUENCE message.
 *
 * @see OpcodeAggregatorSequenceMessage
 */
public class OpcodeAggregatorStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     * Status Code for the requesting message
     */
    public byte status;

    /**
     * 2 bytes
     */
    public int elementAddress;

    /**
     * List of status items with each status item containing an unacknowledged access layer message or empty item
     * Optional
     */
    public List<AggregatorItem> statusItems;


    public OpcodeAggregatorStatusMessage() {
    }

    protected OpcodeAggregatorStatusMessage(Parcel in) {
        status = in.readByte();
        elementAddress = in.readInt();
        statusItems = in.createTypedArrayList(AggregatorItem.CREATOR);
    }

    public static final Creator<OpcodeAggregatorStatusMessage> CREATOR = new Creator<OpcodeAggregatorStatusMessage>() {
        @Override
        public OpcodeAggregatorStatusMessage createFromParcel(Parcel in) {
            return new OpcodeAggregatorStatusMessage(in);
        }

        @Override
        public OpcodeAggregatorStatusMessage[] newArray(int size) {
            return new OpcodeAggregatorStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.elementAddress = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        statusItems = new ArrayList<>();
        while (params.length > index) {
            boolean isLong = (params[index] & 0b01) == 1;
            int len;
            byte[] opAndParams;
            if (isLong) {
                len = ((params[index + 1] & 0xFF) << 8) | ((params[index] & 0xFF) >> 1);
                index += 2;
            } else {
                len = (params[index] & 0xFF) >> 1;
                index += 1;
            }
            if (len == 0) {
                MeshLogger.w("parse err: aggregatorItem len 0");
                continue;
            }
            opAndParams = new byte[len];
            System.arraycopy(params, index, opAndParams, 0, len);
            AggregatorItem item = AggregatorItem.fromBytes(opAndParams);
            MeshLogger.d("parsed Aggregator Item : " + item.toString());
            statusItems.add(item);
            index += len;
        }

    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(elementAddress);
        dest.writeTypedList(statusItems);
    }

    @Override
    public String toString() {
        return "OpcodeAggregatorStatusMessage{" +
                "status=" + status +
                ", elementAddress=" + elementAddress +
                ", statusItems=" + statusItems +
                '}';
    }
}

