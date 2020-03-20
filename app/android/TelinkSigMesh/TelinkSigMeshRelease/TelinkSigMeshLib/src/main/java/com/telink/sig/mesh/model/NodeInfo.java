/********************************************************************************************************
 * @file NodeInfo.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.model;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * 参考 app_provision.h#VC_node_info_t & spec
 * Created by kee on 2018/10/22.
 */
public class NodeInfo implements Serializable {


    //    byte[] rawData;

    // 2 bytes
    public int nodeAdr;

    // 1 byte
    public int elementCnt;

    // 1 byte 用于字节对齐
    public int rsv;

    // 16 bytes
    public byte[] deviceKey;


    /**
     * composition data valid data length
     */
    public int cpsDataLen;


    public CompositionData cpsData;


    public NodeInfo() {

    }

    /**
     * 组装成VC_node_info 用于网络恢复是配置网络信息
     *
     * @return VC_node_info_t
     */
    public byte[] toVCNodeInfo() {
        byte[] result = new byte[404];
        for (int i = 0; i < result.length; i++) {
            result[i] = (byte) 0xFF;
        }
        int index = 0;
        result[index++] = (byte) (nodeAdr & 0xFF);
        result[index++] = (byte) ((nodeAdr >> 8) & 0xFF);

        result[index++] = (byte) (elementCnt & 0xFF);

        result[index++] = (byte) (rsv & 0xFF);

        int keyLen = 16;
        System.arraycopy(deviceKey, 0, result, index, keyLen);
        index += keyLen;

        // record cps len info
        int cpsLenPos = index;

        index += 2;

        result[index++] = (byte) (cpsData.cid & 0xFF);
        result[index++] = (byte) ((cpsData.cid >> 8) & 0xFF);
        result[index++] = (byte) (cpsData.pid & 0xFF);
        result[index++] = (byte) ((cpsData.pid >> 8) & 0xFF);
        result[index++] = (byte) (cpsData.vid & 0xFF);
        result[index++] = (byte) ((cpsData.vid >> 8) & 0xFF);
        result[index++] = (byte) (cpsData.crpl & 0xFF);
        result[index++] = (byte) ((cpsData.crpl >> 8) & 0xFF);
        result[index++] = (byte) (cpsData.features & 0xFF);
        result[index++] = (byte) ((cpsData.features >> 8) & 0xFF);

        for (Element element : cpsData.elements) {
            result[index++] = (byte) (element.location & 0xFF);
            result[index++] = (byte) ((element.location >> 8) & 0xFF);

            result[index++] = (byte) (element.sigNum & 0xFF);
            result[index++] = (byte) (element.vendorNum & 0xFF);

            for (int model : element.sigModels) {
                result[index++] = (byte) (model & 0xFF);
                result[index++] = (byte) ((model >> 8) & 0xFF);
            }


            for (int model : element.vendorModels) {
                result[index++] = (byte) (model & 0xFF);
                result[index++] = (byte) ((model >> 8) & 0xFF);
                result[index++] = (byte) ((model >> 16) & 0xFF);
                result[index++] = (byte) ((model >> 24) & 0xFF);
            }
        }
        int cpsLen = index - cpsLenPos - 2;
        result[cpsLenPos] = (byte) (cpsLen & 0xFF);
        result[cpsLenPos + 1] = (byte) ((cpsLen >> 8) & 0xFF);
        return result;
    }

    public static NodeInfo from(byte[] data) {
        int index = 0;
        NodeInfo info = new NodeInfo();
        info.nodeAdr = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        info.elementCnt = data[index++] & 0xFF;
        info.rsv = data[index++] & 0xFF;
        int keyLen = 16;
        info.deviceKey = new byte[keyLen];
        System.arraycopy(data, index, info.deviceKey, 0, keyLen);
        index += keyLen;

        info.cpsDataLen = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        byte[] cpsData = new byte[info.cpsDataLen];
        System.arraycopy(data, index, cpsData, 0, info.cpsDataLen);
        info.cpsData = CompositionData.from(cpsData);
        return info;
    }


    // 参考 Mesh_V1.0.pdf # 4.2.1
    public static class CompositionData implements Serializable {
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
         * Contains a 16-bit vendor-assigned product version identifier
         */
        public int vid;

        /**
         * Contains a 16-bit value representing the minimum number of replay protection list entries in a device
         */
        public int crpl;

        /**
         * supported features
         */
        public int features;

        public List<Element> elements;

        public CompositionData() {

        }

        public static CompositionData from(byte[] data) {
            int index = 0;
            CompositionData cpsData = new CompositionData();
            cpsData.cid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            cpsData.pid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            cpsData.vid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
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

        public boolean relaySupport() {
            return (features & FEATURE_RELAY) != 0;
        }

        public boolean proxySupport() {
            return (features & FEATURE_PROXY) != 0;
        }

        public boolean friendSupport() {
            return (features & FEATURE_FRIEND) != 0;
        }

        public boolean lowPowerSupport() {
            return (features & FEATURE_LOW_POWER) != 0;
        }
    }

    public static class Element implements Serializable {

        /**
         * 2 bytes
         * Contains a location descriptor
         */
        public int location;

        /**
         * 1 byte
         * Contains a count of SIG Model IDs in this element
         */
        public int sigNum;

        /**
         * 1 byte
         * Contains a count of Vendor Model IDs in this element
         */
        public int vendorNum;

        /**
         * Contains a sequence of NumS SIG Model IDs
         */
        public List<Integer> sigModels;

        /**
         * Contains a sequence of NumV Vendor Model IDs
         */
        public List<Integer> vendorModels;

        public boolean containModel(int sigModelId) {
            if (sigModels == null || sigModels.size() == 0) return false;
            for (int modelId : sigModels) {
                if (sigModelId == modelId) return true;
            }
            return false;
        }
    }
}
