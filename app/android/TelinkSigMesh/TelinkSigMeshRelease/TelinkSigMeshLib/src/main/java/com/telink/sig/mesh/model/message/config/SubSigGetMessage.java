/********************************************************************************************************
 * @file Group.java
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
package com.telink.sig.mesh.model.message.config;

import com.telink.sig.mesh.model.message.ModelMessage;

/**
 * Created by kee on 2019/3/4.
 */

public class SubSigGetMessage implements ModelMessage {

    /**
     * element address
     * 2 bytes
     */
    public int elementAddress;

    /**
     * sig model id
     * 2 bytes
     */
    public int modelId;

    public static SubSigGetMessage createInstance(int elementAddress, int modelId) {
        SubSigGetMessage instance = new SubSigGetMessage();
        instance.elementAddress = elementAddress;
        instance.modelId = modelId;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        return new byte[]{
                (byte) (this.elementAddress & 0xFF), (byte) ((this.elementAddress >> 8) & 0xFF),
                (byte) (this.modelId & 0xFF), (byte) ((this.modelId >> 8) & 0xFF)
        };
    }
}
