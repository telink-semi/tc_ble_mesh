/********************************************************************************************************
 * @file UpdateStatus.java
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

public enum UpdateStatus {

    SUCCESS(0x00, "The message was processed successfully"),

    METADATA_CHECK_FAILED(0x01, "The metadata check failed"),

    INVALID_FIRMWARE_ID(0x02, "The message contains a Firmware ID value that is not expected"),

    OUT_OF_RESOURCES(0x03, "Insufficient resources on the node"),

    BLOB_TRANSFER_BUSY(0x04, "Another BLOB transfer is in progress"),

    INVALID_COMMAND(0x05, "The operation cannot be performed while the server is in the current phase"),

    TEMPORARILY_UNAVAILABLE(0x06, "The server cannot start a firmware update"),

    INTERNAL_ERROR(0x07, "An internal error occurred on the node"),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    UpdateStatus(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static UpdateStatus valueOf(int code) {
        for (UpdateStatus status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
