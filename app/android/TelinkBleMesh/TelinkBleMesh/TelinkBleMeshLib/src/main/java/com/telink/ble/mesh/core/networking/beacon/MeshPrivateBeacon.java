/********************************************************************************************************
 * @file MeshPrivateBeacon.java
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
package com.telink.ble.mesh.core.networking.beacon;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MeshPrivateBeacon extends MeshBeaconPDU {

    private static final int LENGTH_PAYLOAD = 27;

    private static final int MASK_KEY_REFRESH = 0b01;

    private static final int MASK_IV_UPDATE = 0b10;

    private final byte beaconType = BEACON_TYPE_MESH_PRIVATE;

    private byte flags;

    private int ivIndex;

    /**
     * 13 bytes
     * Random number used as an entropy for obfuscation and authentication of the Mesh Private beacon
     */
    private byte[] random;

    /**
     * 5 bytes
     * Obfuscated Private Beacon Data
     */
    private byte[] obfuscatedData;

    /**
     * 8 bytes
     * Authentication tag for the beacon
     */
    private byte[] authenticationTag;

    public static MeshPrivateBeacon from(byte[] payload, byte[] privateBeaconKey) {
        if (payload.length != LENGTH_PAYLOAD) {
            return null;
        }
        int index = 0;
        final byte beaconType = payload[index++];
        if (beaconType != BEACON_TYPE_MESH_PRIVATE) return null;
        MeshPrivateBeacon beacon = new MeshPrivateBeacon();

        final int rdnLen = 13;
        byte[] random = new byte[rdnLen];
        System.arraycopy(payload, index, random, 0, rdnLen);
        index += rdnLen;

        beacon.random = random;
        final int obfLen = 5;
        beacon.obfuscatedData = new byte[obfLen];
        System.arraycopy(payload, index, beacon.obfuscatedData, 0, obfLen);
        index += obfLen;

        final int authLen = 8;
        beacon.authenticationTag = new byte[authLen];
        System.arraycopy(payload, index, beacon.authenticationTag, 0, authLen);

        byte[] c1 = ByteBuffer.allocate(16).order(ByteOrder.BIG_ENDIAN).put((byte) 0x01)
                .put(random).putShort((short) 0x0001).array();

        byte[] s = Encipher.aes(c1, privateBeaconKey);
        byte[] privateBeaconData = new byte[5];
        for (int i = 0; i < privateBeaconData.length; i++) {
            privateBeaconData[i] = (byte) (s[i] ^ beacon.obfuscatedData[i]);
        }

        beacon.flags = privateBeaconData[0];
        beacon.ivIndex = MeshUtils.bytes2Integer(privateBeaconData, 1, 4, ByteOrder.BIG_ENDIAN);

        byte[] authTag = calcAuthTag(privateBeaconData, random, privateBeaconKey);

        if (Arrays.equals(beacon.authenticationTag, authTag)) {
            return beacon;
        } else {
            MeshLogger.w("private beacon auth tag check err");
            return null;
        }
    }

    public static MeshPrivateBeacon createIvUpdatingBeacon(int curIvIndex, byte[] privateBeaconKey, boolean updating) {
        MeshPrivateBeacon meshPrivateBeacon = new MeshPrivateBeacon();

        byte flags = (byte) (updating ? 0b10 : 0);
        meshPrivateBeacon.flags = flags;
        int ivIndex = updating ? curIvIndex + 1 : curIvIndex;
        meshPrivateBeacon.ivIndex = ivIndex;
        byte[] privateBcnData = ByteBuffer.allocate(5).order(ByteOrder.BIG_ENDIAN)
                .put(flags).putInt(ivIndex).array();
        MeshLogger.d("beacon data: " + Arrays.bytesToHexString(privateBcnData));
//        byte[] random = MeshUtils.generateRandom(13);
        byte[] random = Arrays.hexToBytes("435f18f85cf78a3121f58478a5"); //  mesh sig sample

        byte[] authTag = calcAuthTag(privateBcnData, random, privateBeaconKey);
        MeshLogger.d("authTag: " + Arrays.bytesToHexString(authTag));
        byte[] c1 = ByteBuffer.allocate(16).order(ByteOrder.BIG_ENDIAN).put((byte) 0x01)
                .put(random).putShort((short) 0x0001).array();

        /*
         S = e (PrivateBeaconKey, C1)
         */
        byte[] s = Encipher.aes(c1, privateBeaconKey);

        /*
        Obfuscated_Private_Beacon_Data = (S [0-4]) ⊕ (Private Beacon Data)
         */
        byte[] obfData = new byte[5];
        for (int i = 0; i < obfData.length; i++) {
            obfData[i] = (byte) (s[i] ^ privateBcnData[i]);
        }

        meshPrivateBeacon.random = random;
        meshPrivateBeacon.obfuscatedData = obfData;
        meshPrivateBeacon.authenticationTag = authTag;
        return meshPrivateBeacon;
    }

    private static byte[] calcAuthTag(byte[] privateBeaconData, byte[] random, byte[] privateBeaconKey) {

        /*
        B0 = 0x19 || Random || 0x0005
        C0 = 0x01 || Random || 0x0000
        C1 = 0x01 || Random || 0x0001
        P = Private Beacon Data || 0x0000000000000000000000 (11-octets of Zero padding)
         */

        byte[] b0 = ByteBuffer.allocate(16).order(ByteOrder.BIG_ENDIAN).put((byte) 0x19)
                .put(random).putShort((short) 0x0005).array();

        byte[] c0 = ByteBuffer.allocate(16).order(ByteOrder.BIG_ENDIAN).put((byte) 0x01)
                .put(random).putShort((short) 0x0000).array();

        byte[] p = new byte[16];
        java.util.Arrays.fill(p, (byte) 0);
        System.arraycopy(privateBeaconData, 0, p, 0, privateBeaconData.length);
        byte[] t0 = Encipher.aes(b0, privateBeaconKey);
        byte[] t1Input = new byte[t0.length];
        for (int i = 0; i < t0.length; i++) {
            t1Input[i] = (byte) (t0[i] ^ p[i]);
        }

        byte[] t1 = Encipher.aes(t1Input, privateBeaconKey);

        byte[] c0Enc = Encipher.aes(c0, privateBeaconKey);
        byte[] t2 = new byte[t1.length];
        for (int i = 0; i < t1.length; i++) {
            t2[i] = (byte) (t1[i] ^ c0Enc[i]);
        }

        byte[] authTag = new byte[8];
        System.arraycopy(t2, 0, authTag, 0, authTag.length);
        return authTag;
    }

    public byte getBeaconType() {
        return beaconType;
    }


    @Override
    public byte[] toBytes() {
        ByteBuffer buffer1 = ByteBuffer.allocate(LENGTH_PAYLOAD).order(ByteOrder.BIG_ENDIAN);
        buffer1.put(beaconType).put(random).put(obfuscatedData)
                .put(authenticationTag);
        return buffer1.array();
    }

    public byte getFlags() {
        return flags;
    }

    public int getIvIndex() {
        return ivIndex;
    }

    public byte[] getRandom() {
        return random;
    }

    public boolean isIvUpdating() {
        return (flags & MASK_IV_UPDATE) != 0;
    }

    @Override
    public String toString() {
        return "MeshPrivateBeacon{" +
                "beaconType=" + beaconType +
                ", flags=" + flags +
                ", ivIndex=" + ivIndex +
                '}';
    }
}
