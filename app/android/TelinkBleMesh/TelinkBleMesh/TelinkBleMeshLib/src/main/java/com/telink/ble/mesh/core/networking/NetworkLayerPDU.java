/********************************************************************************************************
 * @file NetworkLayerPDU.java
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
package com.telink.ble.mesh.core.networking;


import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class NetworkLayerPDU {

    /**
     * Least significant bit of IV Index
     * 1 bit
     */
    private byte ivi;

    /**
     * network id
     * 7 bits
     * Value derived from the NetKey used to identify the Encryption Key and Privacy Key used to secure this PDU
     */
    private byte nid;

    /**
     * Network Control
     * 1 bit
     * determine if the message is part of a Control MeshCommand or an Access MeshCommand
     */
    private byte ctl;

    /**
     * Time To Live
     * 7 bits
     * value:
     * 0 = has not been relayed and will not be relayed
     * 1 = may have been relayed, but will not be relayed
     * 2 to 126 = may have been relayed and can be relayed
     * 127 = has not been relayed and can be relayed
     */
    private byte ttl;

    /**
     * Sequence Number
     * 24 bits == 3 bytes
     */
    private int seq;

    /**
     * Source Address
     * 16 bits == 2 bytes
     * shall be unicast address
     */
    private int src;

    /**
     * Destination Address
     * 16 bits == 2 bytes
     * shall be a unicast address, a group address, or a virtual address
     */
    private int dst;

    /**
     * Transport Protocol Data Unit
     * 8 to 128 bits
     * When the CTL bit is 0, the TransportPDU field shall be a maximum of 128 bits.
     * When the CTL bit is 1, the TransportPDU field shall be a maximum of 96 bits.
     */
    private byte[] transportPDU;

    /**
     * MeshCommand Integrity Check for Network
     * 32 or 64 bits
     * When the CTL bit is 0, the NetMIC field shall be 32 bits.
     * When the CTL bit is 1, the NetMIC field shall be 64 bits.
     */
//    private int transMic;
    protected NetworkEncryptionSuite encryptionSuite;

    /**
     * This constructor initializes a NetworkLayerPDU object with the specified encryption suite.
     * The encryption suite is used to provide security measures for the network layer protocol data unit.
     *
     * @param encryptionSuite enc
     */
    public NetworkLayerPDU(NetworkEncryptionSuite encryptionSuite) {
        this.encryptionSuite = encryptionSuite;
    }

    /*public NetworkLayerPDU(int ivIndex, byte[] encryptionKey, byte[] privacyKey, int nid) {
        this.ivIndex = ivIndex;
        this.encryptionKey = encryptionKey;
        this.privacyKey = privacyKey;
        this.nid = nid;
    }*/

    /**
     * generates an encrypted payload for a network PDU
     *
     * @return pdu data
     */
    public byte[] generateEncryptedPayload() {
        final byte iviNid = (byte) ((ivi << 7) | (nid & 0x7F));
        final byte ctlTTL = (byte) ((ctl << 7) | (ttl & 0x7F));

        final byte[] encryptedPayload = encryptNetworkPduPayload(transportPDU);
//        MeshLogger.log("encryptedPayload: " + Arrays.bytesToHexString(encryptedPayload, ""));
        final byte[] privacyRandom = createPrivacyRandom(encryptedPayload);
        final byte[] pecb = createPECB(privacyRandom);
//        MeshLogger.log("pecb: " + Arrays.bytesToHexString(pecb, ""));

        final byte[] header = createObfuscatedNetworkHeader(ctlTTL, seq, src, pecb);
//        MeshLogger.log("obfuscateNetworkHeader: " + Arrays.bytesToHexString(header, ""));

        return ByteBuffer.allocate(1 + header.length + encryptedPayload.length).order(ByteOrder.BIG_ENDIAN)
                .put(iviNid)
                .put(header)
                .put(encryptedPayload)
                .array();
    }

    /**
     * creates an obfuscated network header by combining various parameters.
     *
     * @param ctlTTL control TTL (Time to Live)
     * @param sno    sequence number
     * @param src    source address
     * @param pecb   pecb
     * @return header
     */
    private byte[] createObfuscatedNetworkHeader(final byte ctlTTL, int sno, final int src, final byte[] pecb) {

        byte[] seqNo = MeshUtils.integer2Bytes(sno, 3, ByteOrder.BIG_ENDIAN);
        final ByteBuffer buffer = ByteBuffer.allocate(1 + seqNo.length + 2).order(ByteOrder.BIG_ENDIAN);
        buffer.put(ctlTTL);
        buffer.put(seqNo);   //sequence number
        buffer.putShort((short) src);       //source address

        final byte[] headerBuffer = buffer.array();

//        MeshLogger.log("NetworkHeader: " + Arrays.bytesToHexString(headerBuffer, ""));

        final ByteBuffer bufferPECB = ByteBuffer.allocate(6);
        bufferPECB.put(pecb, 0, 6);

        final byte[] obfuscated = new byte[6];
        for (int i = 0; i < 6; i++)
            obfuscated[i] = (byte) (headerBuffer[i] ^ pecb[i]);

        return obfuscated;
    }

    /**
     * This method creates a PECB (Privacy Encryption and Control Block) by combining the privacy random value with other parameters.
     * It first creates a ByteBuffer with a specific size and order.
     * Then, it populates the buffer with a sequence of bytes, including the privacy random value.
     * Finally, it encrypts the buffer using AES encryption with the privacy key and returns the encrypted data.
     *
     * @param privacyRandom random
     * @return PECB
     */
    private byte[] createPECB(byte[] privacyRandom) {
        final ByteBuffer buffer = ByteBuffer.allocate(5 + privacyRandom.length + 4);
        buffer.order(ByteOrder.BIG_ENDIAN);
        buffer.put(new byte[]{0x00, 0x00, 0x00, 0x00, 0x00});
        buffer.putInt(this.encryptionSuite.ivIndex);
        buffer.put(privacyRandom);
        final byte[] temp = buffer.array();
        return Encipher.aes(temp, this.encryptionSuite.privacyKey);
    }

    /**
     * This method extracts the privacy random value from the encrypted upper transport PDU (Protocol Data Unit)
     * and returns it as a byte array.
     *
     * @param encryptedUpperTransportPDU PDU
     * @return random
     */
    private byte[] createPrivacyRandom(byte[] encryptedUpperTransportPDU) {
        final byte[] privacyRandom = new byte[7];
        System.arraycopy(encryptedUpperTransportPDU, 0, privacyRandom, 0, privacyRandom.length);
        return privacyRandom;
    }

    /**
     * This method encrypts the payload of a network PDU by combining it with other parameters.
     * It first generates a network nonce using specific parameters.
     * Then, it creates an unencrypted network payload by combining the destination address and the lower PDU.
     * Finally, it encrypts the unencrypted payload using CCM (Counter with CBC-MAC) mode with the encryption key, network nonce, and a specific MIC (Message Integrity Code) length,
     * and returns the encrypted data.
     *
     * @param lowerPDU lower transport PDU
     * @return payload
     */
    private byte[] encryptNetworkPduPayload(byte[] lowerPDU) {
        byte[] networkNonce = generateNonce();
//        MeshLogger.log("networkNonce: " + Arrays.bytesToHexString(networkNonce, ""));
        final byte[] unencryptedNetworkPayload = ByteBuffer.allocate(2 + lowerPDU.length).order(ByteOrder.BIG_ENDIAN).putShort((short) dst).put(lowerPDU).array();
        return Encipher.ccm(unencryptedNetworkPayload, this.encryptionSuite.encryptionKey, networkNonce, getMicLen(), true);
    }

    /**
     * This method generates a network nonce by
     * combining the CTL (Control) and TTL (Time to Live) values,
     * the sequence number, the source address, and the IV Index (Initialization Vector Index).
     * It returns the generated nonce as a byte array.
     *
     * @return nonce
     */
    protected byte[] generateNonce() {
        byte ctlTTL = (byte) ((ctl << 7) | ttl);
        byte[] seqNo = MeshUtils.integer2Bytes(seq, 3, ByteOrder.BIG_ENDIAN);
        return NonceGenerator.generateNetworkNonce(ctlTTL, seqNo, src, this.encryptionSuite.ivIndex);
    }

    /**
     * This method determines the MIC length based on the value of the CTL.
     * If the CTL is 0, the MIC length is 4; otherwise, it is 8.
     *
     * @return mic length
     */
    private int getMicLen() {
        return ctl == 0 ? 4 : 8;
    }

    /**
     * parse header
     *
     * @param pdu input pdu
     */
    public byte[] parseObfuscatedNetworkHeader(byte[] pdu) {
        final ByteBuffer obfuscatedNetworkBuffer = ByteBuffer.allocate(6);
        obfuscatedNetworkBuffer.order(ByteOrder.BIG_ENDIAN);
        obfuscatedNetworkBuffer.put(pdu, 1, 6);
        final byte[] obfuscatedData = obfuscatedNetworkBuffer.array();

        final ByteBuffer privacyRandomBuffer = ByteBuffer.allocate(7);
        privacyRandomBuffer.order(ByteOrder.BIG_ENDIAN);
        privacyRandomBuffer.put(pdu, 7, 7);
        final byte[] privacyRandom = createPrivacyRandom(privacyRandomBuffer.array());

        final byte[] pecb = createPECB(privacyRandom);
        final byte[] deobfuscatedData = new byte[6];

        for (int i = 0; i < 6; i++)
            deobfuscatedData[i] = (byte) (obfuscatedData[i] ^ pecb[i]);

        return deobfuscatedData;
    }

    /**
     * parse pdu
     *
     * @param pduData data
     * @return parse result
     */
    public boolean parse(byte[] pduData) {

        int iviNid = pduData[0] & 0xFF;
        int ivi = iviNid >> 7;
        int nid = iviNid & 0x7F;
//        MeshLogger.i("ivi -- " + ivi + " nid -- " + nid);
        if (!validateNetworkPdu(ivi, nid)) {
            MeshLogger.i("ivi or nid invalid: ivi -- " + ivi + " nid -- " + nid +
                    " encryptSuit : ivi -- " + encryptionSuite.ivIndex + " nid -- " + encryptionSuite.nid);
            return false;
        }

        byte[] originalHeader = this.parseObfuscatedNetworkHeader(pduData);
        int ctlTtl = originalHeader[0];
        int ctl = (ctlTtl >> 7) & 0x01;
        int ttl = ctlTtl & 0x7F;
        byte[] sequenceNumber = ByteBuffer.allocate(3).order(ByteOrder.BIG_ENDIAN).put(originalHeader, 1, 3).array();
        int src = (((originalHeader[4] & 0xFF) << 8) + (originalHeader[5] & 0xFF));
        this.setIvi((byte) ivi);
        this.setNid((byte) nid);
        this.setCtl((byte) ctl);
        this.setTtl((byte) ttl);
        this.setSeq(MeshUtils.bytes2Integer(sequenceNumber, ByteOrder.BIG_ENDIAN));
        this.setSrc(src);

        byte[] networkNonce = generateNonce();
//        byte[] networkNonce = NonceGenerator.generateNetworkNonce((byte) ctlTtl, sequenceNumber, src, this.encryptionSuite.ivIndex);


        final int dstTransportLen = pduData.length - (1 + originalHeader.length);

        final byte[] dstTransportPayload = new byte[dstTransportLen];
        System.arraycopy(pduData, 7, dstTransportPayload, 0, dstTransportLen);

        // decrypted dest + transport(lower) payload
        final byte[] decDstTransportPayload = Encipher.ccm(dstTransportPayload, this.encryptionSuite.encryptionKey, networkNonce, getMicLen(), false);

        if (decDstTransportPayload == null) {
            MeshLogger.i("network layer decrypt err");
            return false;
        }

        int dstAdr = ((decDstTransportPayload[0] & 0xFF) << 8) | (decDstTransportPayload[1] & 0xFF);

        byte[] lowerTransportPdu = new byte[decDstTransportPayload.length - 2];
        System.arraycopy(decDstTransportPayload, 2, lowerTransportPdu, 0, lowerTransportPdu.length);

        this.dst = dstAdr;
        this.setTransportPDU(lowerTransportPdu);
        return true;
    }

    /**
     * used to validate a network PDU (Protocol Data Unit) based on the given IVI (Initialization Vector Index) and NID (Network Identifier). It checks if the NID matches the encryption suite's NID and if the IVI matches the least significant bit of the encryption suite's IV Index.
     *
     * @param ivi ivi
     * @param nid nid
     * @return check result
     */
    private boolean validateNetworkPdu(int ivi, int nid) {
        return nid == this.encryptionSuite.nid && ivi == (this.encryptionSuite.ivIndex & 0b01);
    }

    /**
     * Returns the IV index of the packet.
     *
     * @return The IV index.
     */
    public byte getIvi() {
        return ivi;
    }

    /**
     * Sets the IV index of the packet.
     *
     * @param ivi The IV index to set.
     */
    public void setIvi(byte ivi) {
        this.ivi = ivi;
    }

    /**
     * Returns the Network ID of the packet.
     *
     * @return The Network ID.
     */
    public byte getNid() {
        return nid;
    }

    /**
     * Sets the Network ID of the packet.
     *
     * @param nid The Network ID to set.
     */
    public void setNid(byte nid) {
        this.nid = nid;
    }

    /**
     * Returns the Control field of the packet.
     *
     * @return The Control field.
     */
    public byte getCtl() {
        return ctl;
    }

    /**
     * Sets the Control field of the packet.
     *
     * @param ctl The Control field to set.
     */
    public void setCtl(byte ctl) {
        this.ctl = ctl;
    }

    /**
     * Returns the Time to Live (TTL) of the packet.
     *
     * @return The TTL.
     */
    public byte getTtl() {
        return ttl;
    }

    /**
     * Sets the Time to Live (TTL) of the packet.
     *
     * @param ttl The TTL to set.
     */
    public void setTtl(byte ttl) {
        this.ttl = ttl;
    }

    /**
     * Returns the Sequence number of the packet.
     *
     * @return The Sequence number.
     */
    public int getSeq() {
        return seq;
    }

    /**
     * Sets the Sequence number of the packet.
     *
     * @param seq The Sequence number to set.
     */
    public void setSeq(int seq) {
        this.seq = seq;
    }

    /**
     * Returns the Source address of the packet.
     *
     * @return The Source address.
     */
    public int getSrc() {
        return src;
    }

    /**
     * Sets the Source address of the packet.
     *
     * @param src The Source address to set.
     */
    public void setSrc(int src) {
        this.src = src;
    }

    /**
     * Returns the Destination address of the packet.
     *
     * @return The Destination address.
     */
    public int getDst() {
        return dst;
    }

    /**
     * Sets the Destination address of the packet.
     *
     * @param dst The Destination address to set.
     */
    public void setDst(int dst) {
        this.dst = dst;
    }

    /**
     * Returns the Transport Protocol Data Unit (PDU) of the packet.
     *
     * @return The Transport PDU.
     */
    public byte[] getTransportPDU() {
        return transportPDU;
    }

    /**
     * Sets the Transport Protocol Data Unit (PDU) of the packet.
     *
     * @param transportPDU The Transport PDU to set.
     */
    public void setTransportPDU(byte[] transportPDU) {
        this.transportPDU = transportPDU;
    }

    public static class NetworkEncryptionSuite {

        // for encryption
        public int ivIndex;

        protected byte[] encryptionKey;


        protected byte[] privacyKey;

        protected int nid;

        /**
         * This class represents a network encryption suite used for secure communication.
         * It encapsulates the necessary parameters for encryption, such as the initialization vector index,
         * encryption key, privacy key, and network ID.
         *
         * @param ivIndex       The index of the initialization vector used for encryption.
         * @param encryptionKey The encryption key used for secure communication.
         * @param privacyKey    The privacy key used for secure communication.
         * @param nid           The network ID associated with the encryption suite.
         */
        public NetworkEncryptionSuite(int ivIndex, byte[] encryptionKey, byte[] privacyKey, int nid) {
            this.ivIndex = ivIndex;
            this.encryptionKey = encryptionKey;
            this.privacyKey = privacyKey;
            this.nid = nid;
        }
    }


    /**
     * Returns a string representation of the NetworkLayerPDU object.
     * The string includes the values of its instance variables.
     * The transportPDU byte array is converted into a hexadecimal string.
     *
     * @return a string representation of the NetworkLayerPDU object.
     */
    @Override
    public String toString() {
        return "NetworkLayerPDU{" +
                "ivi=" + ivi +
                ", nid=" + nid +
                ", ctl=" + ctl +
                ", ttl=" + ttl +
                ", seq=0x" + Integer.toHexString(seq) +
                ", src=" + src +
                ", dst=" + dst +
                ", transportPDU=" + Arrays.bytesToHexString(transportPDU) +
                '}';
    }
}

