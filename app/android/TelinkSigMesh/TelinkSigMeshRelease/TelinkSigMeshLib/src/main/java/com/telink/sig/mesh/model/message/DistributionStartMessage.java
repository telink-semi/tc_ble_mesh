/********************************************************************************************************
 * @file DistributionStartMessage.java
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
package com.telink.sig.mesh.model.message;

/**
 * Distribution Start in mesh ota
 * Created by kee on 2019/5/8.
 */
// sample: a3ff0000000000000001 b60a 1102 210000ff 00c0 0101
public class DistributionStartMessage implements ModelMessage {

    public int cid = 0x0211;

    public int fwId = 0xFF000021;

    // set nodes into group
    public int groupAddress = 0xC001;

    // max 180 nodes
    public int[] nodeAddresses;

    public DistributionStartMessage() {
    }

    public static DistributionStartMessage getDefault(int[] nodeAddresses) {
        DistributionStartMessage message = new DistributionStartMessage();
        message.nodeAddresses = nodeAddresses;
        return message;
    }


    public static DistributionStartMessage createInstance(int cid, int fwId, int groupAddress, int[] nodeAddresses) {
        DistributionStartMessage message = new DistributionStartMessage();
        message.cid = cid;
        message.fwId = fwId;
        message.groupAddress = groupAddress;
        message.nodeAddresses = nodeAddresses;
        return message;
    }

    @Override
    public byte[] toBytes() {
        if (nodeAddresses == null) return null;
        final int len = 2 // cid
                + 4 // fw id
                + 2 // group address
                + (2 * nodeAddresses.length) // node addresses
                ;
        int index = 0;
        byte[] re = new byte[len];
        re[index++] = (byte) cid;
        re[index++] = (byte) (cid >> 8);

        re[index++] = (byte) fwId;
        re[index++] = (byte) (fwId >> 8);
        re[index++] = (byte) (fwId >> 16);
        re[index++] = (byte) (fwId >> 24);

        re[index++] = (byte) groupAddress;
        re[index++] = (byte) (groupAddress >> 8);

        for (int nodeAdr : nodeAddresses) {
            re[index++] = (byte) nodeAdr;
            re[index++] = (byte) (nodeAdr >> 8);
        }

        return re;
    }
}
