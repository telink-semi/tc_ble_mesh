/********************************************************************************************************
 * @file DistributionStatus.java
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
package com.telink.ble.mesh.core.message.firmwareupdate;

/**
 * The DistributionStatus enum represents the possible statuses of a message during the distribution process.
 * Each status has a corresponding code and description.
 */
public enum DistributionStatus {


    SUCCESS(0x00, "The message was processed successfully"),

    INSUFFICIENT_RESOURCES(0x01, "Insufficient resources on the node"),

    WRONG_PHASE(0x02, "The operation cannot be performed while the server is in the current phase"),

    INTERNAL_ERROR(0x03, "An internal error occurred on the node"),


    FIRMWARE_NOT_FOUND(0x04, "The requested firmware image is not stored on the Distributor"),

    INVALID_APPKEY_INDEX(0x05, "The AppKey identified by the AppKey Index is not known to the node"),

    RECEIVERS_LIST_EMPTY(0x06, "There are no Updating nodes in the Distribution Receivers List state"),

    BUSY_WITH_DISTRIBUTION(0x07, "Another firmware image distribution is in progress"),

    BUSY_WITH_UPLOAD(0x08, "Another upload is in progress"),

    URI_NOT_SUPPORTED(0x09, "The URI scheme name indicated by the Update URI is not supported"),

    URI_MALFORMED(0x0A, "The format of the Update URI is invalid"),

    URI_UNREACHABLE(0x0B, "The URI is unreachable."),

    NEW_FIRMWARE_NOT_AVAILABLE(0x0C, "The Check Firmware OOB procedure did not find any new firmware."),

    SUSPEND_FAILED(0x0D, "The suspension of the Distribute Firmware procedure failed."),

    UNKNOWN_ERROR(0xFF, "unknown error");

    public final int code;
    public final String desc;

    /**
     * Constructor for the DistributionStatus enum.
     *
     * @param code The code representing the status.
     * @param desc The description of the status.
     */
    DistributionStatus(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    /**
     * Returns the DistributionStatus enum value corresponding to the given code.
     * If no matching status is found, returns UNKNOWN_ERROR.
     *
     * @param code The code to find the corresponding status for.
     * @return The DistributionStatus enum value.
     */
    public static DistributionStatus valueOf(int code) {
        for (DistributionStatus status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
