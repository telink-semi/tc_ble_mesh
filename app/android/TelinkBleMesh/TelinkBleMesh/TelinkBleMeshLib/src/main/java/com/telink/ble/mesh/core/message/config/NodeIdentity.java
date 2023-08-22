/********************************************************************************************************
 * @file NodeIdentity.java
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
