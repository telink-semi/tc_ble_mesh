/********************************************************************************************************
 * @file OtaPacketParser.java
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
package com.telink.ble.mesh.util;


/**
 * The OtaPacketParser class is responsible for parsing OTA (Over-The-Air) packets.
 * It provides methods to set the data, clear the object's state, and retrieve the next packet.
 * The class keeps track of the progress and index of the packets.
 */
public class OtaPacketParser {
    private int total;
    private int index = -1;
    private byte[] data;
    private int progress;

    /**
     * Sets the data for this object.
     * Clears any existing data and assigns the given byte array to the data variable.
     * Calculates the total number of blocks based on the length of the data.
     * If the length is divisible by 16, the total number of blocks is set to length/16.
     * If the length is not divisible by 16, the total number of blocks is set to the floor value of length/16 + 1.
     *
     * @param data The byte array representing the data to be parsed.
     */
    public void set(byte[] data) {
        this.clear();
        this.data = data;
        int length = this.data.length;
        int size = 16;
        if (length % size == 0) {
            total = length / size;
        } else {
            total = (int) Math.floor(length / size + 1);
        }
    }

    /**
     * Clears/reset the state of the object.
     * Sets the progress, total, index, and data variables to their initial values or null.
     * By calling this method, the object's progress tracking will be reset and any previously stored data will be cleared.
     */
    public void clear() {
        this.progress = 0;
        this.total = 0;
        this.index = -1;
        this.data = null;
    }

    /**
     * Checks if there is a next packet available.
     *
     * @return true if there is a next packet available, false otherwise.
     */
    public boolean hasNextPacket() {
        return this.total > 0 && (this.index + 1) < this.total;
    }

    /**
     * Checks if the current packet is the last packet.
     *
     * @return true if the current packet is the last packet, false otherwise.
     */
    public boolean isLast() {
        return (this.index + 1) == this.total;
    }

    /**
     * Retrieves the index of the next packet.
     *
     * @return The index of the next packet.
     */
    public int getNextPacketIndex() {
        return this.index + 1;
    }

    /**
     * Retrieves the next packet.
     * Updates the index to the next packet.
     *
     * @return The byte array representing the next packet.
     */
    public byte[] getNextPacket() {
        int index = this.getNextPacketIndex();
        byte[] packet = this.getPacket(index);
        this.index = index;
        return packet;
    }

    /**
     * Retrieves the packet at the given index.
     *
     * @param index The index of the packet to retrieve.
     * @return The byte array representing the packet.
     */
    public byte[] getPacket(int index) {
        int length = this.data.length;
        int size = 16;
        int packetSize;
        if (length > size) {
            if ((index + 1) == this.total) {
                packetSize = length - index * size;
            } else {
                packetSize = size;
            }
        } else {
            packetSize = length;
        }
        packetSize = packetSize + 4;
        byte[] packet = new byte[20];
        for (int i = 0; i < 20; i++) {
            packet[i] = (byte) 0xFF;
        }
        System.arraycopy(this.data, index * size, packet, 2, packetSize - 4);
        this.fillIndex(packet, index);
        int crc = this.crc16(packet);
        this.fillCrc(packet, crc);
        MeshLogger.log("ota packet ---> index : " + index + " total : " + this.total + " crc : " + crc + " content : " + Arrays.bytesToHexString(packet, ":"));
        return packet;
    }

    /**
     * Retrieves the check packet for the next packet.
     *
     * @return The byte array representing the check packet.
     */
    public byte[] getCheckPacket() {
        byte[] packet = new byte[16];
        for (int i = 0; i < 16; i++) {
            packet[i] = (byte) 0xFF;
        }
        int index = this.getNextPacketIndex();
        this.fillIndex(packet, index);
        int crc = this.crc16(packet);
        this.fillCrc(packet, crc);
        MeshLogger.log("ota check packet ---> index : " + index + " crc : " + crc + " content : " + Arrays.bytesToHexString(packet, ":"));
        return packet;
    }

    /**
     * Fills the index value in the packet.
     *
     * @param packet The byte array representing the packet.
     * @param index  The index value to fill.
     */
    public void fillIndex(byte[] packet, int index) {
        int offset = 0;
        packet[offset++] = (byte) (index & 0xFF);
        packet[offset] = (byte) (index >> 8 & 0xFF);
    }

    /**
     * Fills the CRC (Cyclic Redundancy Check) value in the packet.
     *
     * @param packet The byte array representing the packet.
     * @param crc    The CRC value to fill.
     */
    public void fillCrc(byte[] packet, int crc) {
        int offset = packet.length - 2;
        packet[offset++] = (byte) (crc & 0xFF);
        packet[offset] = (byte) (crc >> 8 & 0xFF);
    }

    /**
     * Calculates the CRC (Cyclic Redundancy Check) value for the packet.
     *
     * @param packet The byte array representing the packet.
     * @return The CRC value.
     */
    public int crc16(byte[] packet) {
        int length = packet.length - 2;
        short[] poly = new short[]{0, (short) 0xA001};
        int crc = 0xFFFF;
        int ds;
        for (int j = 0; j < length; j++) {
            ds = packet[j];
            for (int i = 0; i < 8; i++) {
                crc = (crc >> 1) ^ poly[(crc ^ ds) & 1] & 0xFFFF;
                ds = ds >> 1;
            }
        }
        return crc;
    }

    /**
     * Invalidates the progress if it has changed.
     * Calculates the progress based on the current index and total number of packets.
     *
     * @return true if the progress has changed, false otherwise.
     */
    public boolean invalidateProgress() {
        float a = this.getNextPacketIndex();
        float b = this.total;
        int progress = (int) Math.floor((a / b * 100));
//        MeshLogger.log("ota_progress: a:"+ a + " b:"+ b +" progress:"+ progress);
        if (progress == this.progress)
            return false;
        this.progress = progress;
        return true;
    }

    /**
     * Retrieves the progress of the parsing process.
     *
     * @return The progress as a percentage.
     */
    public int getProgress() {
        return this.progress;
    }

    /**
     * Retrieves the index of the current packet.
     *
     * @return The index of the current packet.
     */
    public int getIndex() {
        return this.index;
    }
}
