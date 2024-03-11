/********************************************************************************************************
 * @file LargeCompositionDataStatusMessage.java
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
package com.telink.ble.mesh.core.message.largecps;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.entity.CompositionData;

import java.nio.ByteOrder;

/**
 * A LARGE_COMPOSITION_DATA_STATUS message is an unacknowledged message used to report a
 * portion of a page of the Composition Data
 * The LargeCompositionDataStatusMessage class represents an unacknowledged message that is used to report a portion of a page of the Composition Data.
 * It extends the StatusMessage class and implements the Parcelable interface for serialization and deserialization.
 * This message contains the page number, offset, total size, and the Composition Data for the identified page.
 */
public class LargeCompositionDataStatusMessage extends StatusMessage implements Parcelable {

    /**
     * The page number of the Composition Data.
     */
    private byte page;

    /**
     * The offset value.
     */
    private int offset;

    /**
     * The total size of the Composition Data.
     */
    private int totalSize;

    /**
     * The Composition Data for the identified page.
     */
    private CompositionData compositionData;

    /**
     * Constructs a new LargeCompositionDataStatusMessage object with default values.
     */
    public LargeCompositionDataStatusMessage() {
    }

    /**
     * Constructs a new LargeCompositionDataStatusMessage object by deserializing it from a Parcel.
     *
     * @param in The Parcel object to read the data from.
     */
    protected LargeCompositionDataStatusMessage(Parcel in) {
        page = in.readByte();
        compositionData = in.readParcelable(CompositionData.class.getClassLoader());
    }

    /**
     * A Creator object that is used to create instances of the LargeCompositionDataStatusMessage class from a Parcel.
     */
    public static final Creator<LargeCompositionDataStatusMessage> CREATOR = new Creator<LargeCompositionDataStatusMessage>() {
        @Override
        public LargeCompositionDataStatusMessage createFromParcel(Parcel in) {
            return new LargeCompositionDataStatusMessage(in);
        }

        @Override
        public LargeCompositionDataStatusMessage[] newArray(int size) {
            return new LargeCompositionDataStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and populates the fields of the LargeCompositionDataStatusMessage object.
     *
     * @param params The byte array containing the data to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        page = params[index++];
        offset = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        totalSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        byte[] cpsData = new byte[params.length - 1];
        System.arraycopy(params, index, cpsData, 0, cpsData.length);
        compositionData = CompositionData.from(cpsData);
    }

    /**
     * Returns the description of the Parcelable object.
     *
     * @return The description of the Parcelable object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Serializes the LargeCompositionDataStatusMessage object to a Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags for the serialization.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(page);
        dest.writeParcelable(compositionData, flags);
    }

    /**
     * Returns the page number of the Composition Data.
     *
     * @return The page number of the Composition Data.
     */
    public byte getPage() {
        return page;
    }

    /**
     * Returns the Composition Data for the identified page.
     *
     * @return The Composition Data for the identified page.
     */
    public CompositionData getCompositionData() {
        return compositionData;
    }

    /**
     * Returns the offset value.
     *
     * @return The offset value.
     */
    public int getOffset() {
        return offset;
    }

    /**
     * Returns the total size of the Composition Data.
     *
     * @return The total size of the Composition Data.
     */
    public int getTotalSize() {
        return totalSize;
    }
}