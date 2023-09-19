/********************************************************************************************************
 * @file TransferMode.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

/**
 * The Transfer Mode state is a 2-bit value that indicates the mode of the BLOB transfer
 */
public enum TransferMode {

    NONE(0x00, "No Active Transfer"),

    PUSH(0x01, "Push BLOB Transfer Mode"),

    PULL(0x02, "Pull BLOB Transfer Mode"),

    Prohibited(0x03, "Prohibited");

    public final int value;
    public final String desc;

    TransferMode(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }

    public static TransferMode valueOf(int mode) {
        for (TransferMode status : values()) {
            if (status.value == mode) return status;
        }
        return Prohibited;
    }
}
