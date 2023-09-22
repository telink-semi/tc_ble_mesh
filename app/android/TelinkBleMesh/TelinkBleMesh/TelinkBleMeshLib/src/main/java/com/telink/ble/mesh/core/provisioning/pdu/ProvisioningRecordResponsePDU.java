/********************************************************************************************************
 * @file ProvisioningRecordResponsePDU.java
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
package com.telink.ble.mesh.core.provisioning.pdu;


import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.util.Arrays;

import java.nio.ByteOrder;


/**
 * This class represents a Provisioning Record Response PDU.
 * It implements the ProvisioningStatePDU interface.
 * It contains fields for the raw data, status, record ID, fragment offset, total length, and data.
 * It provides methods to convert the PDU to bytes and to parse bytes into a PDU object.
 */
public class ProvisioningRecordResponsePDU implements ProvisioningStatePDU {


    /**
     * status success
     */
    public static final byte STATUS_SUCCESS = 0x00;

    /**
     * status Requested Record Is Not Present
     */
    public static final byte STATUS_RECORD_NOT_PRESENT = 0x01;

    /**
     * status Requested Offset Is Out Of Bounds
     */
    public static final byte STATUS_OFFSET_OUT_OF_BOUNDS = 0x02;
    /**
     * The raw data of the PDU.
     */
    public byte[] rawData;
    /**
     * Indicates whether or not the request was handled successfully
     */
    public byte status;

    /**
     * The ID of the record.
     */
    public int recordID;
    /**
     * The fragment offset of the record.
     */
    public int fragmentOffset;
    /**
     * The total length of the record.
     */
    public int totalLength;
    /**
     * The data of the record.
     */
    public byte[] data;

    /**
     * Constructs a ProvisioningRecordResponsePDU object from a byte array.
     *
     * @param data The byte array representing the PDU.
     * @return The ProvisioningRecordResponsePDU object.
     */
    public static ProvisioningRecordResponsePDU fromBytes(byte[] data) {

        int index = 0;
        ProvisioningRecordResponsePDU responsePDU = new ProvisioningRecordResponsePDU();
        responsePDU.rawData = data;
        responsePDU.status = data[index++];
        responsePDU.recordID = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
        index += 2;

        responsePDU.fragmentOffset = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
        index += 2;

        responsePDU.totalLength = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
        index += 2;

        responsePDU.data = new byte[data.length - index];
        System.arraycopy(data, index, responsePDU.data, 0, responsePDU.data.length);
        return responsePDU;
    }

    /**
     * Gets the state of the PDU.
     *
     * @return The state of the PDU.
     */

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RECORD_RESPONSE;
    }

    /**
     * Converts the PDU to a byte array.
     *
     * @return The byte array representation of the PDU.
     */
    @Override
    public byte[] toBytes() {
        return rawData;
    }

    /**
     * Converts the PDU object to a string representation.
     *
     * @return The string representation of the PDU object.
     */
    @Override
    public String toString() {
        return "ProvisioningRecordResponsePDU{" +
                "status=" + status +
                ", recordID=" + recordID +
                ", fragmentOffset=" + fragmentOffset +
                ", totalLength=" + totalLength +
                ", data=" + Arrays.bytesToHexString(data) +
                '}';
    }
}
