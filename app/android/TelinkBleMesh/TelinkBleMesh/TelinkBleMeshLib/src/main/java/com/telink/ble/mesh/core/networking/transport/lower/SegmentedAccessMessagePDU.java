/********************************************************************************************************
 * @file SegmentedAccessMessagePDU.java
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
 * The class represents a segmented access message Protocol Data Unit (PDU) used in a mesh network.
 * It contains various attributes and methods to handle and manipulate the PDU.
 */
public class SegmentedAccessMessagePDU extends LowerTransportPDU {


    /**
     * 1 = Segmented MeshCommand
     */
    private final byte seg = 1;

    /**
     * Application Key Flag
     */
    private byte akf;

    /**
     * Application key identifier
     */
    private byte aid;

    /**
     * Size of TransMIC
     */
    private int szmic;

    /**
     * Least significant bits of SeqAuth
     */
    private int seqZero;

    /**
     * Segment Offset number
     */
    private int segO;

    /**
     * Last Segment number
     */
    private int segN;

    /**
     * Segment m of the Upper Transport Access PDU
     */
    private byte[] segmentM;

    /**
     * Returns the type of this PDU.
     *
     * @return The type of this PDU (TYPE_SEGMENTED_ACCESS_MESSAGE).
     */
    @Override
    public int getType() {
        return TYPE_SEGMENTED_ACCESS_MESSAGE;
    }

    /**
     * Indicates whether this PDU is segmented.
     *
     * @return True, as this PDU is segmented.
     */
    @Override
    public boolean segmented() {
        return true;
    }

    /**
     * convert to byte array
     *
     * @return byte array
     */
    @Override
    public byte[] toByteArray() {
        int headerLength = 4;
        final int akfAid = ((akf << 6) | aid);
        int payloadLength = segmentM.length;
        ByteBuffer resultBuffer = ByteBuffer.allocate(headerLength + payloadLength).order(ByteOrder.BIG_ENDIAN);
        resultBuffer.put((byte) ((seg << 7) | akfAid));
        resultBuffer.put((byte) ((szmic << 7) | ((seqZero >> 6) & 0x7F)));
        resultBuffer.put((byte) (((seqZero << 2) & 0xFC) | ((segO >> 3) & 0x03)));
        resultBuffer.put((byte) (((segO << 5) & 0xE0) | ((segN) & 0x1F)));
        resultBuffer.put(segmentM);
        return resultBuffer.array();
    }

    /**
     * Parses the given NetworkLayerPDU and extracts the information to populate this PDU.
     *
     * @param networkLayerPDU The NetworkLayerPDU to parse.
     * @return True if the parsing was successful, false otherwise.
     */
    public boolean parse(NetworkLayerPDU networkLayerPDU) {
        byte[] lowerTransportPdu = networkLayerPDU.getTransportPDU();
        this.akf = (byte) ((lowerTransportPdu[0] >> 6) & 0x01);
        this.aid = (byte) (lowerTransportPdu[0] & 0x3F);
        this.szmic = (lowerTransportPdu[1] >> 7) & 0x01;
        this.seqZero = ((lowerTransportPdu[1] & 0x7F) << 6) | ((lowerTransportPdu[2] & 0xFC) >> 2);
        this.segO = ((lowerTransportPdu[2] & 0x03) << 3) | ((lowerTransportPdu[3] & 0xE0) >> 5);
        this.segN = ((lowerTransportPdu[3]) & 0x1F);

        this.segmentM = new byte[lowerTransportPdu.length - 4];
        System.arraycopy(lowerTransportPdu, 4, this.segmentM, 0, this.segmentM.length);
        return this.segmentM != null && this.segmentM.length >= 1;
    }


    /**
     * Returns the value of the 'seg' attribute.
     *
     * @return The 'seg' attribute value.
     */
    public byte getSeg() {
        return seg;
    }

    /**
     * Returns the value of the 'akf' attribute.
     *
     * @return The 'akf' attribute value.
     */
    public byte getAkf() {
        return akf;
    }

    /**
     * Sets the value of the 'akf' attribute.
     *
     * @param akf The new value for the 'akf' attribute.
     */
    public void setAkf(byte akf) {
        this.akf = akf;
    }

    /**
     * Returns the value of the 'aid' attribute.
     *
     * @return The 'aid' attribute value.
     */
    public byte getAid() {
        return aid;
    }

    /**
     * Sets the value of the 'aid' attribute.
     *
     * @param aid The new value for the 'aid' attribute.
     */
    public void setAid(byte aid) {
        this.aid = aid;
    }

    /**
     * Returns the value of the 'szmic' attribute.
     *
     * @return The 'szmic' attribute value.
     */
    public int getSzmic() {
        return szmic;
    }

    /**
     * Sets the value of the 'szmic' attribute.
     *
     * @param szmic The new value for the 'szmic' attribute.
     */
    public void setSzmic(int szmic) {
        this.szmic = szmic;
    }

    /**
     * Returns the value of the 'seqZero' attribute.
     *
     * @return The 'seqZero' attribute value.
     */
    public int getSeqZero() {
        return seqZero;
    }

    /**
     * Sets the value of the 'seqZero' attribute.
     *
     * @param seqZero The new value for the 'seqZero' attribute.
     */
    public void setSeqZero(int seqZero) {
        this.seqZero = seqZero;
    }

    /**
     * Returns the value of the 'segO' attribute.
     *
     * @return The 'segO' attribute value.
     */
    public int getSegO() {
        return segO;
    }

    /**
     * Sets the value of the 'segO' attribute.
     *
     * @param segO The new value for the 'segO' attribute.
     */
    public void setSegO(int segO) {
        this.segO = segO;
    }

    /**
     * Returns the value of the 'segN' attribute.
     *
     * @return The 'segN' attribute value.
     */
    public int getSegN() {
        return segN;
    }

    /**
     * Sets the value of the 'segN' attribute.
     *
     * @param segN The new value for the 'segN' attribute.
     */
    public void setSegN(int segN) {
        this.segN = segN;
    }

    /**
     * Returns the value of the 'segmentM' attribute.
     *
     * @return The 'segmentM' attribute value.
     */
    public byte[] getSegmentM() {
        return segmentM;
    }

    /**
     * Sets the value of the 'segmentM' attribute.
     *
     * @param segmentM The new value for the 'segmentM' attribute.
     */
    public void setSegmentM(byte[] segmentM) {
        this.segmentM = segmentM;
    }
}
