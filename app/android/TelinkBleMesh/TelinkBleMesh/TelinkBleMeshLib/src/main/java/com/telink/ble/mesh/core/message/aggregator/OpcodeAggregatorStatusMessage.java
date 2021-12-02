/********************************************************************************************************
 * @file AppKeyStatusMessage.java
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
package com.telink.ble.mesh.core.message.aggregator;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.OpcodeType;
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

