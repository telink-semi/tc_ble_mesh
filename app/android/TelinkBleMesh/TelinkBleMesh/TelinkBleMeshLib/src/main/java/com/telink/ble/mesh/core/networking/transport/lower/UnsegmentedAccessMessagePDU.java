/********************************************************************************************************
 * @file UnsegmentedAccessMessagePDU.java
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
package com.telink.ble.mesh.core.networking.transport.lower;

import com.telink.ble.mesh.core.networking.NetworkLayerPDU;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/9.
 */

/**
 * This class represents an unsegmented access message PDU for lower transport layer.
 * It extends the LowerTransportPDU class.
 * <p>
 * The unsegmented access message PDU consists of the following fields:
 * - seg: 1 bit indicating whether the PDU is segmented or not (0: unsegmented, 1: segmented)
 * - akf: 1 bit indicating the application key flag
 * - aid: 6 bits representing the application key identifier
 * - upperTransportPDU: 40 to 120 bits of upper transport PDU data
 * <p>
 * The class provides methods to access and modify these fields, as well as methods to convert the PDU to a byte array and parse a byte array to populate the PDU fields.
 */
public class UnsegmentedAccessMessagePDU extends LowerTransportPDU {

    // segment
    /**
     * 1 bit
     * 0: unsegmented , means a single PDU
     * 1: segment
     */
    private final byte seg = 0;

    /**
     * 1 bit
     * Application Key Flag
     */
    private byte akf;

    /**
     * 6 bits
     * Application key identifier
     */
    private byte aid;

    /**
     * 40 to 120 bits
     */
    private byte[] upperTransportPDU;

    /**
     * Default constructor.
     */
    public UnsegmentedAccessMessagePDU() {

    }

    /**
     * Constructor with parameters.
     *
     * @param akf               the application key flag
     * @param aid               the application key identifier
     * @param upperTransportPDU the upper transport PDU data
     */
    public UnsegmentedAccessMessagePDU(byte akf, byte aid, byte[] upperTransportPDU) {
        this.akf = akf;
        this.aid = aid;
        this.upperTransportPDU = upperTransportPDU;
    }

    /**
     * Returns the type of the PDU.
     *
     * @return the type of the PDU
     */
    @Override
    public int getType() {
        return TYPE_UNSEGMENTED_ACCESS_MESSAGE;
    }

    /**
     * Checks if the PDU is segmented or not.
     *
     * @return true if the PDU is segmented, false otherwise
     */
    @Override
    public boolean segmented() {
        return false;
    }

    /**
     * Converts the PDU to a byte array.
     *
     * @return the byte array representation of the PDU
     */
    @Override
    public byte[] toByteArray() {
        byte oct0 = (byte) ((seg << 7) | (akf << 6) | aid);
        ByteBuffer lowerTransportBuffer = ByteBuffer.allocate(1 + upperTransportPDU.length).order(ByteOrder.BIG_ENDIAN);
        lowerTransportBuffer.put(oct0);
        lowerTransportBuffer.put(upperTransportPDU);
        return lowerTransportBuffer.array();
    }

    /**
     * Parses a network layer PDU to populate the fields of the unsegmented access message PDU.
     *
     * @param networkLayerPDU the network layer PDU to parse
     * @return true if the parsing is successful, false otherwise
     */
    public boolean parse(NetworkLayerPDU networkLayerPDU) {
        byte[] lowerTransportData = networkLayerPDU.getTransportPDU();
        byte header = lowerTransportData[0]; //Lower transport pdu starts here
        this.akf = (byte) ((header >> 6) & 0x01);
        this.aid = (byte) (header & 0x3F);
        byte[] upperTransportPDU = new byte[lowerTransportData.length - 1];
        System.arraycopy(lowerTransportData, 1, upperTransportPDU, 0, upperTransportPDU.length);
        this.upperTransportPDU = upperTransportPDU;
        return upperTransportPDU.length != 0;
    }


    /**
     * Returns the seg field value.
     *
     * @return the seg field value
     */
    public byte getSeg() {
        return seg;
    }

    /**
     * Returns the akf field value.
     *
     * @return the akf field value
     */
    public byte getAkf() {
        return akf;
    }

    /**
     * Sets the akf field value.
     *
     * @param akf the new akf field value
     */
    public void setAkf(byte akf) {
        this.akf = akf;
    }

    /**
     * Returns the aid field value.
     *
     * @return the aid field value
     */
    public byte getAid() {
        return aid;
    }

    /**
     * Sets the aid field value.
     *
     * @param aid the new aid field value
     */
    public void setAid(byte aid) {
        this.aid = aid;
    }

    /**
     * Returns the upperTransportPDU field value.
     *
     * @return the upperTransportPDU field value
     */
    public byte[] getUpperTransportPDU() {
        return upperTransportPDU;
    }

    /**
     * Sets the upperTransportPDU field value.
     *
     * @param upperTransportPDU the new upperTransportPDU field value
     */
    public void setUpperTransportPDU(byte[] upperTransportPDU) {
        this.upperTransportPDU = upperTransportPDU;
    }
}
