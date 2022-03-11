/********************************************************************************************************
 * @file TransferStatus.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

public enum TransferStatus {

    SUCCESS(0x00, "The message was processed successfully"),

    INVALID_BLOCK_NUMBER(0x01, "The Block Number field value is not within the range of blocks being transferred"),

    INVALID_BLOCK_SIZE(0x02, "The block size is smaller than the size indicated by the Min Block Size Log state or is larger than the size indicated by the Max Block Size Log state."),

    INVALID_CHUNK_SIZE(0x03, " The chunk size exceeds the size indicated by the Max Chunk Size state, or the number of chunks exceeds the number specified by the Max Total Chunks state."),

    INVALID_STATE(0x04, "The operation cannot be performed while the server is in the current phase."),

    INVALID_PARAMETER(0x05, "A parameter value in the message cannot be accepted."),

    WRONG_BLOB_ID(0x06, "The message contains a BLOB ID value that is not expected."),

    BLOB_TOO_LARGE(0x07, "There is not enough space available in memory to receive the BLOB."),

    UNSUPPORTED_TRANSFER_MODE(0x08, "The transfer mode is not supported by the BLOB Transfer Server model."),

    INTERNAL_ERROR(0x09, "An internal error occurred on the node"),

    UNKNOWN_ERROR(0x0A, "unknown error");

    public final int code;
    public final String desc;

    TransferStatus(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    public static TransferStatus valueOf(int code) {
        for (TransferStatus status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
