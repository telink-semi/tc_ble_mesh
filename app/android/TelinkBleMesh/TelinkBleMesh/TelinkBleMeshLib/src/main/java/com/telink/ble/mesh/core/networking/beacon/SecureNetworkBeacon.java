/********************************************************************************************************
 * @file SecureNetworkBeacon.java
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

/**
 * This class represents a secure network beacon in a mesh network.
 */
public class SecureNetworkBeacon extends MeshBeaconPDU {

    private static final int LENGTH_PAYLOAD = 22;

    private static final int MASK_KEY_REFRESH = 0b01;

    private static final int MASK_IV_UPDATE = 0b10;

    private final byte beaconType = BEACON_TYPE_SECURE_NETWORK;

    /**
     * Contains the Key Refresh Flag and IV Update Flag
     */
    private byte flags;

    /**
     * Contains the value of the Network ID
     * 8 bytes
     */
    private byte[] networkID;

    /**
     * Contains the current IV Index
     * 4 bytes
     * big endian
     */
    private int ivIndex;

    /**
     * Authenticates security network beacon
     * 8 bytes
     */
    private byte[] authenticationValue;


    /**
     * Returns whether the beacon is currently performing key refreshing.
     *
     * @return true if key refreshing is enabled, false otherwise.
     */
    public boolean isKeyRefreshing() {
        return (flags & MASK_KEY_REFRESH) != 0;
    }

    /**
     * Returns whether the beacon is currently performing IV updating.
     *
     * @return true if IV updating is enabled, false otherwise.
     */
    public boolean isIvUpdating() {
        return (flags & MASK_IV_UPDATE) != 0;
    }

    /**
     * Creates a SecureNetworkBeacon object from the given payload and beacon key.
     *
     * @param payload   the payload of the beacon.
     * @param beaconKey the beacon key used for authentication.
     * @return a SecureNetworkBeacon object if the payload is valid and authentication is successful, null otherwise.
     */
    public static SecureNetworkBeacon from(byte[] payload, byte[] beaconKey) {
        if (payload.length != LENGTH_PAYLOAD) {
            return null;
        }
        int index = 0;
        final byte beaconType = payload[index++];
        if (beaconType != BEACON_TYPE_SECURE_NETWORK) return null;
        SecureNetworkBeacon beacon = new SecureNetworkBeacon();
        beacon.flags = payload[index++];
        beacon.networkID = new byte[8];
        System.arraycopy(payload, index, beacon.networkID, 0, beacon.networkID.length);
        index += beacon.networkID.length;
        beacon.ivIndex = MeshUtils.bytes2Integer(payload, index, 4, ByteOrder.BIG_ENDIAN);
        index += 4;
        beacon.authenticationValue = new byte[8];
        System.arraycopy(payload, index, beacon.authenticationValue, 0, beacon.authenticationValue.length);
        byte[] authCal = generateAuthValue(beacon, beaconKey);
        if (Arrays.equals(authCal, beacon.authenticationValue)) {
            return beacon;
        } else {
            MeshLogger.w("network beacon check err");
            return null;
        }
    }

    /**
     * Creates a SecureNetworkBeacon object for IV updating.
     *
     * @param curIvIndex the current IV index.
     * @param networkId  the network ID.
     * @param beaconKey  the beacon key used for authentication.
     * @param updating   true if IV updating is enabled, false otherwise.
     * @return a SecureNetworkBeacon object for IV updating.
     */
    public static SecureNetworkBeacon createIvUpdatingBeacon(int curIvIndex, byte[] networkId, byte[] beaconKey, boolean updating) {
        SecureNetworkBeacon networkBeacon = new SecureNetworkBeacon();
        networkBeacon.flags = (byte) (updating ? 0b10 : 0);
        networkBeacon.networkID = networkId;
        networkBeacon.ivIndex = updating ? curIvIndex + 1 : curIvIndex;
        networkBeacon.authenticationValue = generateAuthValue(networkBeacon, beaconKey);
        return networkBeacon;
    }


    /**
     * Generates the authentication value for the given SecureNetworkBeacon and beacon key.
     *
     * @param networkBeacon the SecureNetworkBeacon object.
     * @param beaconKey     the beacon key used for authentication.
     * @return the authentication value.
     */
    private static byte[] generateAuthValue(SecureNetworkBeacon networkBeacon, byte[] beaconKey) {
        final int calLen = 1 + 8 + 4;
        ByteBuffer buffer = ByteBuffer.allocate(calLen).order(ByteOrder.BIG_ENDIAN);
        buffer.put(networkBeacon.flags);
        buffer.put(networkBeacon.networkID);
        buffer.putInt(networkBeacon.ivIndex);
        byte[] auth = Encipher.aesCmac(buffer.array(), beaconKey);
        byte[] authCal = new byte[8];
        System.arraycopy(auth, 0, authCal, 0, authCal.length);
        return authCal;
    }


    /**
     * Returns a string representation of the SecureNetworkBeacon object.
     *
     * @return a string representation of the object.
     */

    @Override
    public String toString() {
        return "SecureNetworkBeacon{" +
                "beaconType=" + beaconType +
                ", flags=" + flags +
                ", networkID=" + Arrays.bytesToHexString(networkID, "") +
                ", ivIndex=0x" + String.format("%08X", ivIndex) +
                ", authenticationValue=" + Arrays.bytesToHexString(authenticationValue, "") +
                '}';
    }

    /**
     * Returns the beacon type.
     *
     * @return the beacon type.
     */
    public byte getBeaconType() {
        return beaconType;
    }

    /**
     * Returns the flags.
     *
     * @return the flags.
     */
    public byte getFlags() {
        return flags;
    }

    /**
     * Sets the flags.
     *
     * @param flags the flags to set.
     */
    public void setFlags(byte flags) {
        this.flags = flags;
    }

    /**
     * Returns the network ID.
     *
     * @return the network ID.
     */
    public byte[] getNetworkID() {
        return networkID;
    }

    /**
     * Sets the network ID.
     *
     * @param networkID the network ID to set.
     */
    public void setNetworkID(byte[] networkID) {
        this.networkID = networkID;
    }

    /**
     * Returns the IV index.
     *
     * @return the IV index.
     */
    public int getIvIndex() {
        return ivIndex;
    }

    /**
     * Sets the IV index.
     *
     * @param ivIndex the IV index to set.
     */
    public void setIvIndex(int ivIndex) {
        this.ivIndex = ivIndex;
    }

    /**
     * Returns the authentication value.
     *
     * @return the authentication value.
     */
    public byte[] getAuthenticationValue() {
        return authenticationValue;
    }

    /**
     * Sets the authentication value.
     *
     * @param authenticationValue the authentication value to set.
     */
    public void setAuthenticationValue(byte[] authenticationValue) {
        this.authenticationValue = authenticationValue;
    }

    /**
     * Converts the SecureNetworkBeacon object to a byte array.
     *
     * @return the byte array representation of the object.
     */
    @Override
    public byte[] toBytes() {
        ByteBuffer buffer1 = ByteBuffer.allocate(LENGTH_PAYLOAD).order(ByteOrder.BIG_ENDIAN);
        buffer1.put(beaconType).put(flags).put(networkID)
                .putInt(ivIndex).put(authenticationValue);
        return buffer1.array();

    }
}
