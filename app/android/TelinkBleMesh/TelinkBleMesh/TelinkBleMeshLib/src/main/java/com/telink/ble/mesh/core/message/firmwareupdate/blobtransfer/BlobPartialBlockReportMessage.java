/********************************************************************************************************
 * @file BlobPartialBlockReportMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.charset.Charset;
import java.util.ArrayList;

/**
 * This class represents a message for reporting partial blocks of a blob.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * The message contains a list of chunks that are requested by the server.
 * The chunks are encoded using UTF-8 encoding.
 */
public class BlobPartialBlockReportMessage extends StatusMessage implements Parcelable {

    private ArrayList<Integer> encodedMissingChunks;

    /**
     * Default constructor for BlobPartialBlockReportMessage.
     */
    public BlobPartialBlockReportMessage() {
    }

    /**
     * Constructor for BlobPartialBlockReportMessage that initializes the object from a Parcel.
     *
     * @param in The Parcel object from which to read the encoded missing chunks list.
     */
    protected BlobPartialBlockReportMessage(Parcel in) {
        encodedMissingChunks = new ArrayList<>();
        in.readList(encodedMissingChunks, null);
    }

    /**
     * Creator for BlobPartialBlockReportMessage that generates instances of the class from a Parcel.
     */
    public static final Creator<BlobPartialBlockReportMessage> CREATOR = new Creator<BlobPartialBlockReportMessage>() {
        @Override
        public BlobPartialBlockReportMessage createFromParcel(Parcel in) {
            return new BlobPartialBlockReportMessage(in);
        }

        @Override
        public BlobPartialBlockReportMessage[] newArray(int size) {
            return new BlobPartialBlockReportMessage[size];
        }
    };

    /**
     * Parses the byte array parameter to populate the encoded missing chunks list.
     *
     * @param params The byte array containing the encoded missing chunks.
     */
    @Override
    public void parse(byte[] params) {
        encodedMissingChunks = new ArrayList<>();
        String decodeMissingChunks = new String(params, Charset.forName("UTF-8"));
        for (char c : decodeMissingChunks.toCharArray()) {
            encodedMissingChunks.add(c & 0xFFFF);
        }
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return The bitmask value, which is 0 in this case.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the encoded missing chunks list to the Parcel object.
     *
     * @param dest  The Parcel object to write the encoded missing chunks list to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeList(encodedMissingChunks);
    }

    /**
     * Returns the encoded missing chunks list.
     *
     * @return The encoded missing chunks list.
     */
    public ArrayList<Integer> getEncodedMissingChunks() {
        return encodedMissingChunks;
    }
}