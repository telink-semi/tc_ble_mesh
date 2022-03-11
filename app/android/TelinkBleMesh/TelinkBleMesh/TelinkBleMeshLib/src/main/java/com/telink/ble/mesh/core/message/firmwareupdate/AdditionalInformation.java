/********************************************************************************************************
 * @file AdditionalInformation.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.firmwareupdate;

public enum AdditionalInformation {


    No_changes(0x00, "No changes to node composition data"),

    CPS_CHANGED_1(0x01, "Node composition data changed. The node does not support remote provisioning."),

    CPS_CHANGED_2(0x02, "Node composition data changed, and remote provisioning is supported. " +
            "The node supports remote provisioning and composition data page 0x80. Page 0x80 contains different composition data than page 0x0."),

    NODE_UNPROVISIONED(0x03, "Node unprovisioned. The node is unprovisioned after successful application of a verified firmware image."),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    AdditionalInformation(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static AdditionalInformation valueOf(int code) {
        for (AdditionalInformation status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
