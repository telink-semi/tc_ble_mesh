/********************************************************************************************************
 * @file ScanReportStatusMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;


/**
 * This class represents a status message for a scan report.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class ScanReportStatusMessage extends StatusMessage implements Parcelable {
    private byte rssi; // The received signal strength indicator (RSSI)
    private byte[] uuid; // The universally unique identifier (UUID)
    private int oob; // The out-of-band (OOB) value

    /**
     * Default constructor for the ScanReportStatusMessage class.
     */
    public ScanReportStatusMessage() {
    }

    /**
     * Constructor for the ScanReportStatusMessage class that takes a Parcel as input.
     * It reads the values from the Parcel and assigns them to the corresponding fields.
     *
     * @param in The Parcel input
     */
    protected ScanReportStatusMessage(Parcel in) {
        rssi = in.readByte();
        uuid = in.createByteArray();
        oob = in.readInt();
    }

    /**
     * The creator for the ScanReportStatusMessage class that implements the Parcelable.Creator interface.
     * It creates a new instance of the class from a Parcel input.
     */
    public static final Creator<ScanReportStatusMessage> CREATOR = new Creator<ScanReportStatusMessage>() {
        @Override
        public ScanReportStatusMessage createFromParcel(Parcel in) {
            return new ScanReportStatusMessage(in);
        }

        @Override
        public ScanReportStatusMessage[] newArray(int size) {
            return new ScanReportStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameter and assigns the values to the corresponding fields.
     *
     * @param params The byte array parameter to parse
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.rssi = params[index++];
        this.uuid = new byte[16];
        System.arraycopy(params, index, this.uuid, 0, this.uuid.length);
        index += this.uuid.length;
        this.oob = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return The bitmask
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the object into a Parcel.
     * It writes the values of the fields into the Parcel.
     *
     * @param dest  The Parcel to write to
     * @param flags Additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(rssi);
        dest.writeByteArray(uuid);
        dest.writeInt(oob);
    }

    /**
     * Returns the received signal strength indicator (RSSI) value.
     *
     * @return The RSSI value
     */
    public byte getRssi() {
        return rssi;
    }

    /**
     * Returns the universally unique identifier (UUID) value.
     *
     * @return The UUID value
     */
    public byte[] getUuid() {
        return uuid;
    }

    /**
     * Returns the out-of-band (OOB) value.
     *
     * @return The OOB value
     */
    public int getOob() {
        return oob;
    }
}
