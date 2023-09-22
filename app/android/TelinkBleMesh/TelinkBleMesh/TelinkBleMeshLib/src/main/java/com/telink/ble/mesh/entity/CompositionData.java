/********************************************************************************************************
 * @file CompositionData.java
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
package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * CompositionData class represents the composition data of a Bluetooth device in a mesh network.
 * It implements the Serializable and Parcelable interfaces for data serialization and deserialization.
 */
public class CompositionData implements Serializable, Parcelable {
    /**
     * bit 0 Relay feature support: 0 = False, 1 = True
     */
    private static final int FEATURE_RELAY = 0b0001;

    /**
     * bit 1 Proxy feature support: 0 = False, 1 = True
     */
    private static final int FEATURE_PROXY = 0b0010;

    /**
     * bit 2 Friend feature support: 0 = False, 1 = True
     */
    private static final int FEATURE_FRIEND = 0b0100;

    /**
     * bit 3 Low Power feature support: 0 = False, 1 = True
     */
    private static final int FEATURE_LOW_POWER = 0b1000;


    /**
     * Contains a 16-bit company identifier assigned by the Bluetooth SIG
     */
    public int cid;

    /**
     * Contains a 16-bit vendor-assigned product identifier
     */
    public int pid;

    /**
     * Big-endian
     * Contains a 16-bit vendor-assigned product version identifier
     */
    public int vid;

    /**
     * Contains a 16-bit value representing the minimum number of replay protection list entries in a device
     */
    public int crpl;

    /**
     * supported features
     * 16 bits
     */
    public int features;
    /**
     * The list of elements present in the device.
     */
    public List<Element> elements;
    /**
     * The raw byte array representation of the composition data.
     */
    public byte[] raw;

    /**
     * Default constructor for the CompositionData class.
     */
    public CompositionData() {

    }

    /**
     * Parcelable constructor for the CompositionData class.
     *
     * @param in The Parcel object containing the composition data.
     */
    protected CompositionData(Parcel in) {
        cid = in.readInt();
        pid = in.readInt();
        vid = in.readInt();
        crpl = in.readInt();
        features = in.readInt();
    }

    /**
     * Parcelable creator for the CompositionData class.
     */
    public static final Creator<CompositionData> CREATOR = new Creator<CompositionData>() {
        @Override
        public CompositionData createFromParcel(Parcel in) {
            return new CompositionData(in);
        }

        @Override
        public CompositionData[] newArray(int size) {
            return new CompositionData[size];
        }
    };

    /**
     * Creates a CompositionData object from the given byte array.
     *
     * @param data The byte array representing the composition data.
     * @return The CompositionData object created from the byte array.
     */
    public static CompositionData from(byte[] data) {

        int index = 0;
        CompositionData cpsData = new CompositionData();
        cpsData.raw = data;
        cpsData.cid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.pid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
//        cpsData.vid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.vid = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN); // // big endian
        index += 2;

        cpsData.crpl = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.features = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);

        cpsData.elements = new ArrayList<>();
        while (index < data.length) {
            Element element = new Element();
            element.location = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            element.sigNum = (data[index++] & 0xFF);
            element.vendorNum = (data[index++] & 0xFF);

            element.sigModels = new ArrayList<>();
            for (int i = 0; i < element.sigNum; i++) {
                element.sigModels.add((data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8));
            }

            element.vendorModels = new ArrayList<>();
            for (int j = 0; j < element.vendorNum; j++) {
                //sample 11 02 01 00 cid: 11 02 modelId: 01 00 -> 0x00010211
                element.vendorModels.add(((data[index++] & 0xFF)) | ((data[index++] & 0xFF) << 8) |
                        ((data[index++] & 0xFF) << 16) | ((data[index++] & 0xFF) << 24));
            }

            cpsData.elements.add(element);
        }

        return cpsData;
    }

    /**
     * Converts the CompositionData object to a byte array.
     *
     * @return The byte array representation of the CompositionData object.
     */
    public byte[] toBytes() {
        if (raw != null) return raw;
        byte[] re = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) cid)
                .putShort((short) pid)
                .put(MeshUtils.integer2Bytes(vid, 2, ByteOrder.BIG_ENDIAN))
                .putShort((short) crpl)
                .putShort((short) features).array();
        int eleLen;
        ByteBuffer bf;
        for (Element ele : elements) {
            eleLen = 4 + ele.sigNum * 2 + ele.vendorNum * 4;
            bf = ByteBuffer.allocate(re.length + eleLen).order(ByteOrder.LITTLE_ENDIAN)
                    .put(re)
                    .putShort((short) ele.location)
                    .put((byte) ele.sigNum)
                    .put((byte) ele.vendorNum);
            for (int modelId : ele.sigModels) {
                bf.putShort((short) modelId);
            }
            for (int modelId : ele.vendorModels) {
                bf.putInt(modelId);
            }
            re = bf.array();
        }
        return re;
    }

    /**
     * Retrieves all the models present in the CompositionData object.
     *
     * @param configExcept Flag indicating whether to exclude config models.
     * @return A list of all the models present in the CompositionData object.
     * @deprecated This method is deprecated.
     */
    public List<Integer> getAllModels(boolean configExcept) {
        if (elements == null) return null;
        List<Integer> models = new ArrayList<>();
        for (Element ele : elements) {
            if (ele.sigModels != null) {
                if (!configExcept) {
                    models.addAll(ele.sigModels);
                } else {
                    for (int modelId : ele.sigModels) {
                        if (!MeshSigModel.useDeviceKeyForEnc(modelId)) {
                            models.add(modelId);
                        }
                    }
                }

            }
            if (ele.vendorModels != null) {
                models.addAll(ele.vendorModels);
            }
        }

        return models;
    }

    /**
     * Retrieves the element offset of the specified model.
     *
     * @param modelId The model identifier.
     * @return The element offset of the specified model, or -1 if not found.
     */
    public int getElementOffset(int modelId) {
        int offset = 0;
        for (Element ele : elements) {
            if (ele.sigModels != null && ele.sigModels.contains(modelId)) {
                return offset;
            }
            if (ele.vendorModels != null && ele.vendorModels.contains(modelId)) {
                return offset;
            }
            offset++;
        }
        return -1;
    }

    /**
     * Checks if the device supports the relay feature.
     *
     * @return True if the device supports the relay feature, false otherwise.
     */
    public boolean relaySupport() {
        return (features & FEATURE_RELAY) != 0;
    }

    /**
     * Checks if the device supports the proxy feature.
     *
     * @return True if the device supports the proxy feature, false otherwise.
     */
    public boolean proxySupport() {
        return (features & FEATURE_PROXY) != 0;
    }

    /**
     * Checks if the device supports the friend feature.
     *
     * @return True if the device supports the friend feature, false otherwise.
     */
    public boolean friendSupport() {
        return (features & FEATURE_FRIEND) != 0;
    }

    /**
     * Checks if the device supports the low power feature.
     *
     * @return True if the device supports the low power feature, false otherwise.
     */
    public boolean lowPowerSupport() {
        return (features & FEATURE_LOW_POWER) != 0;
    }

    /**
     * Parcelable method for describing the contents of the CompositionData object.
     *
     * @return An integer value representing the contents description.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Parcelable method for writing the CompositionData object to a Parcel.
     *
     * @param dest  The Parcel object to write the CompositionData object to.
     * @param flags Additional flags for writing the object.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(cid);
        dest.writeInt(pid);
        dest.writeInt(vid);
        dest.writeInt(crpl);
        dest.writeInt(features);
        dest.writeTypedList(elements);
    }

    /**
     * Returns a string representation of the CompositionData object.
     *
     * @return A string representation of the CompositionData object.
     */
    @Override
    public String toString() {
        StringBuilder elementInfo = new StringBuilder();
        Element element;
        for (int i = 0; i < elements.size(); i++) {
            element = elements.get(i);
            elementInfo.append("element ").append(i).append(" : \n");
            elementInfo.append("location: ").append(element.location).append("\n");
            elementInfo.append("SIG models-").append(element.sigModels.size()).append(":");
            String sig;
            for (int j = 0; j < element.sigModels.size(); j++) {
                sig = String.format("%04X", element.sigModels.get(j));
                elementInfo.append(sig).append(" ");
            }
            elementInfo.append("\n");
//            elementInfo.append("VENDOR\n");
            elementInfo.append("Vendor models-").append(element.vendorModels.size()).append(":");
            for (int j = 0; j < element.vendorModels.size(); j++) {
                elementInfo.append(String.format("%08X", element.vendorModels.get(j))).append("\t");
            }
            elementInfo.append("\n");
        }

        return "CompositionData{" +
                "cid=" + String.format("%04X", cid) +
                ", pid=" + String.format("%04X", pid) +
                ", vid=" + String.format("%04X", vid) +
                ", crpl=" + String.format("%04X", crpl) +
                ", features=" + String.format("%04X", features) +
                ", elements=\n" + elementInfo +
                '}';
    }
}


