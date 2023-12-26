/********************************************************************************************************
 * @file NonceGenerator.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * This class is responsible for generating nonces used in network authentication and encryption.
 * It provides methods to generate network, application, device, and proxy nonces.
 * Nonces are used in various encryption and authentication processes in network communication.
 */
public class NonceGenerator {

    private static final int NONCE_LENGTH = 13;

    /**
     * Used with an encryption key for network authentication and encryption
     */
    private static final byte NONCE_TYPE_NETWORK = 0x00;

    /**
     * Used with an application key for upper transport authentication and encryption
     */
    private static final byte NONCE_TYPE_APPLICATION = 0x01;

    /**
     * Used with a device key for upper transport authentication and encryption
     */
    private static final byte NONCE_TYPE_DEVICE = 0x02;

    /**
     * Used with an encryption key for proxy authentication and encryption
     */
    private static final byte NONCE_TYPE_PROXY = 0x03;

    /**
     * Used with an encryption key for proxy authentication and encryption
     */
    private static final byte NONCE_TYPE_SOLICITATION = 0x04;

    private static final byte NONCE_PADDING = 0x00;

    public static byte[] generateNetworkNonce(byte ctlTTL, byte[] sequenceNumber, int src, int ivIndex) {
        final ByteBuffer networkNonce = ByteBuffer.allocate(13).order(ByteOrder.BIG_ENDIAN);
        networkNonce.put(NONCE_TYPE_NETWORK); //Nonce typeValue
        networkNonce.put(ctlTTL); // CTL and TTL
        networkNonce.put(sequenceNumber);
        networkNonce.putShort((short) src);
        networkNonce.put(new byte[]{0x00, 0x00}); //PADDING
        networkNonce.putInt(ivIndex);
        return networkNonce.array();
    }

    /**
     * output application or device nonce
     *
     * @param aszmic     SZMIC if a Segmented Access message or 0 for all other message formats
     * @param accessType if Application or Device command
     */
    public static byte[] generateAccessNonce(byte aszmic, byte[] sequenceNumber, int src, int dst, int ivIndex, AccessType accessType) {
        ByteBuffer accessNonceBuf = ByteBuffer.allocate(NONCE_LENGTH).order(ByteOrder.BIG_ENDIAN);
        byte type = accessType == AccessType.APPLICATION ? NONCE_TYPE_APPLICATION : NONCE_TYPE_DEVICE;
        accessNonceBuf.put(type); //Nonce typeValue
        accessNonceBuf.put((byte) ((aszmic << 7) | NONCE_PADDING)); //ASZMIC (SZMIC if a segmented access message) and PAD
        accessNonceBuf.put(sequenceNumber);
        accessNonceBuf.putShort((short) src);
        accessNonceBuf.putShort((short) dst);
        accessNonceBuf.putInt(ivIndex);
        return accessNonceBuf.array();
    }

    /**
     * output proxy nonce
     */
    public static byte[] generateProxyNonce(byte[] sequenceNumber, int src, int ivIndex) {
        ByteBuffer nonceBuffer = ByteBuffer.allocate(NONCE_LENGTH);
        nonceBuffer.put(NONCE_TYPE_PROXY); //Nonce typeValue
        nonceBuffer.put(NONCE_PADDING); //PAD
        nonceBuffer.put(sequenceNumber);
        nonceBuffer.putShort((short) src);
        nonceBuffer.put(new byte[]{NONCE_PADDING, NONCE_PADDING});
        nonceBuffer.putInt(ivIndex);
        return nonceBuffer.array();
    }

    /**
     * output proxy nonce
     */
    public static byte[] generateSolicitationNonce(byte[] sequenceNumber, int src) {
        ByteBuffer nonceBuffer = ByteBuffer.allocate(NONCE_LENGTH);
        nonceBuffer.put(NONCE_TYPE_SOLICITATION); //Nonce typeValue
        nonceBuffer.put(NONCE_PADDING); //PAD
        nonceBuffer.put(sequenceNumber);
        nonceBuffer.putShort((short) src);
        nonceBuffer.put(new byte[6]);
        return nonceBuffer.array();
    }
}
