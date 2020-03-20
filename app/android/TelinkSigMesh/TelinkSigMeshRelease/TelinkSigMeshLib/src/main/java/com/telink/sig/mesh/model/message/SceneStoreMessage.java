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
package com.telink.sig.mesh.model.message;

/**
 * Created by kee on 2019/2/25.
 */

public class SceneStoreMessage implements ModelMessage {
    /**
     * Scene Number
     * 2 bytes
     * The number of the scene to be stored.
     */
    public int id;


    public static SceneStoreMessage createInstance(int id) {
        SceneStoreMessage instance = new SceneStoreMessage();
        instance.id = id;
        return instance;
    }

    @Override
    public byte[] toBytes() {
        return new byte[]{(byte) (this.id & 0xFF), (byte) (this.id >> 8)};
    }
}
