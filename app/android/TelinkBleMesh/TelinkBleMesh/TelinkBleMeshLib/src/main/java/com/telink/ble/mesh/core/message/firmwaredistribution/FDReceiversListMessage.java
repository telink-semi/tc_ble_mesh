/********************************************************************************************************
 * @file FDReceiversListMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatePhase;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * response to a Firmware Distribution Receivers Get message
 *
 * @see FDReceiversGetMessage
 */
public class FDReceiversListMessage extends StatusMessage implements Parcelable {

    /**
     * Receivers List Count
     * The number of entries in the Distribution Receivers List state
     * 2 bytes
     */
    private int receiversListCount;


    /**
     * First Index
     * 2 bytes
     */
    private int firstIndex;

    /**
     * List of entries
     * Optional
     */
    private List<DistributionReceiver> receiversList;

    public FDReceiversListMessage() {
    }

    /**
     * Constructor for the FDReceiversListMessage class that initializes the instance variables from a Parcel.
     *
     * @param in The Parcel containing the receiversListCount and firstIndex.
     */
    protected FDReceiversListMessage(Parcel in) {
        receiversListCount = in.readInt();
        firstIndex = in.readInt();
    }

    /**
     * Creator constant for the FDReceiversListMessage class.
     */
    public static final Creator<FDReceiversListMessage> CREATOR = new Creator<FDReceiversListMessage>() {
        @Override
        public FDReceiversListMessage createFromParcel(Parcel in) {
            return new FDReceiversListMessage(in);
        }

        @Override
        public FDReceiversListMessage[] newArray(int size) {
            return new FDReceiversListMessage[size];
        }
    };

    /**
     * Parses the byte array to extract the params.
     *
     * @param params The byte array containing the params.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.receiversListCount = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.firstIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        receiversList = new ArrayList<>();
        DistributionReceiver receiver;
        while (params.length - index >= 5) {
            receiver = new DistributionReceiver();
            int misc = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
            receiver.address = misc & 0x7FFF;
            receiver.retrievedUpdatePhase = (byte) ((misc >> 15) & 0x0F);
            receiver.updateStatus = (byte) ((misc >> 19) & 0x07);
            receiver.transferStatus = (byte) ((misc >> 22) & 0x0F);
            receiver.transferProgress = (byte) ((misc >> 26) & 0x3F);
//            MeshLogger.d(String.format(Locale.getDefault(), " list  -- %d - %d -%d -%d -%d", receiver.address,
//                    receiver.retrievedUpdatePhase,
//                    receiver.updateStatus, receiver.transferStatus, receiver.transferProgress));
            index += 4;
            receiver.imageIndex = params[index++];
            receiversList.add(receiver);
//            if (params.length - index < 5) break;
        }
    }

    /**
     * Implements the describeContents() method from the Parcelable interface.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the params to the Parcel.
     *
     * @param dest  The Parcel to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(receiversListCount);
        dest.writeInt(firstIndex);
    }

    /**
     * Returns receivers list count.
     *
     * @return The receivers list count.
     */
    public int getReceiversListCount() {
        return receiversListCount;
    }

    /**
     * Returns firstIndex.
     *
     * @return The firstIndex.
     */
    public int getFirstIndex() {
        return firstIndex;
    }

    /**
     * Returns receiversList.
     *
     * @return The receiversList.
     */
    public List<DistributionReceiver> getReceiversList() {
        return receiversList;
    }

    static public class DistributionReceiver {
        /**
         * Address
         * 15 least significant bits of the unicast address of the Updating node
         * 15 bits
         */
        public int address;
        /**
         * Retrieved Update Phase
         * Retrieved Update Phase state of the Updating node
         * 4 bits
         *
         * @see UpdatePhase
         */
        public byte retrievedUpdatePhase;
        /**
         * Update Status
         * Status of the last operation with the Firmware Update Server
         * 3	bits
         */
        public byte updateStatus;

        /**
         * Transfer Status
         * Status of the last operation with the BLOB Transfer Server
         * 4 bits
         */
        public byte transferStatus;


        /**
         * Transfer Progress
         * Progress of the BLOB transfer in 2 percent increments
         * 6 bits
         */
        public byte transferProgress;

        /**
         * Update Firmware Image Index
         * Index of the firmware image on the Firmware Information List state that is being updated.
         * 8 bits
         */
        public byte imageIndex;

        /**
         * Returns a string representation of the DistributionReceiver object.
         *
         * @return A string representation of the DistributionReceiver object.
         */
        @Override
        public String toString() {
            return "DistributionReceiver{" +
                    "address=" + address +
                    ", retrievedUpdatePhase=" + retrievedUpdatePhase +
                    ", updateStatus=" + updateStatus +
                    ", transferStatus=" + transferStatus +
                    ", transferProgress=" + transferProgress +
                    ", imageIndex=" + imageIndex +
                    '}';
        }
    }
}
