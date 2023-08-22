/********************************************************************************************************
 * @file AggregatorItem.java
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
import com.telink.ble.mesh.core.message.OpcodeType;
import com.telink.ble.mesh.util.Arrays;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class AggregatorItem implements Parcelable {
    public static final int LENGTH_SHORT = 0x00;

    public static final int LENGTH_LONG = 0x01;
    /**
     * bit0: 0: Length_Short field is present 1: Length_Long field is present
     * if opcodeAndParameters's length <= 127, length is 8 bits
     * if opcodeAndParameters's length > 127, length is 16 bits
     */
//    public int length;

    public int opcode;

    public byte[] parameters;

    public AggregatorItem() {
    }

    public AggregatorItem(int opcode, byte[] parameters) {
        this.opcode = opcode;
        this.parameters = parameters;
    }

    protected AggregatorItem(Parcel in) {
        opcode = in.readInt();
        parameters = in.createByteArray();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(opcode);
        dest.writeByteArray(parameters);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<AggregatorItem> CREATOR = new Creator<AggregatorItem>() {
        @Override
        public AggregatorItem createFromParcel(Parcel in) {
            return new AggregatorItem(in);
        }

        @Override
        public AggregatorItem[] newArray(int size) {
            return new AggregatorItem[size];
        }
    };

    public byte[] toBytes() {
        int opcodeLen = OpcodeType.getByFirstByte((byte) opcode).length;
        int len = parameters == null ? opcodeLen : (opcodeLen + parameters.length);
        final boolean isLong = len > 127;
        int bufLen = (isLong ? 2 : 1) + len;
        ByteBuffer buffer = ByteBuffer.allocate(bufLen).order(ByteOrder.LITTLE_ENDIAN);
        len <<= 1 | (isLong ? 1 : 0);
        if (isLong) {
            buffer.putShort((short) len);
        } else {
            buffer.put((byte) len);
        }
        buffer.put(MeshUtils.integer2Bytes(opcode, opcodeLen, ByteOrder.LITTLE_ENDIAN));
        if (parameters != null) {
            buffer.put(parameters);
        }
        return buffer.array();
    }

    public static AggregatorItem fromBytes(byte[] data) {
        OpcodeType opType = OpcodeType.getByFirstByte(data[0]);

        int opcode = 0;
        int index = 0;
        for (int i = 0; i < opType.length; i++) {
            opcode |= (data[index++] & 0xFF) << (8 * i);
        }

        final int paramLen = data.length - opType.length;
        byte[] params = new byte[paramLen];
        System.arraycopy(data, index, params, 0, paramLen);


        AggregatorItem item = new AggregatorItem();
        item.opcode = opcode;
        item.parameters = params;
        return item;
    }

    @Override
    public String toString() {
        return "AggregatorItem{" +
                String.format("opcode=%04X", opcode) +
                ", parameters=" + Arrays.bytesToHexString(parameters) +
                '}';
    }
}
