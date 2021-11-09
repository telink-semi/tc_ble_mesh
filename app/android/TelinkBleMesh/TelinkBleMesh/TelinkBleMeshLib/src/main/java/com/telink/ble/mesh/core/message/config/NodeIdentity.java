/********************************************************************************************************
 * @file NodeIdentity.java
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
package com.telink.ble.mesh.core.message.config;

/**
 * The Node Identity state determines if a node that supports the Mesh Proxy Service is advertising on a subnet using Node Identity messages.
 */
public enum NodeIdentity {

    STOPPED((byte) 0, "Node Identity for a subnet is stopped"),

    RUNNING((byte) 1, "Node Identity for a subnet is running"),

    UNSUPPORTED((byte) 2, "Node Identity is not supported"),

    UNKNOWN_ERROR((byte) 0xFF, "unknown error");

    public final byte code;
    public final String desc;

    NodeIdentity(byte code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static NodeIdentity valueOf(int code) {
        for (NodeIdentity status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
