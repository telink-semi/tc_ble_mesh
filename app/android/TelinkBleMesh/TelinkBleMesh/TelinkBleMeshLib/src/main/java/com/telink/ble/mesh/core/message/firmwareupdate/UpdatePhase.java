/********************************************************************************************************
 * @file UpdatePhase.java
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
 * This enum represents the different phases of an update process.
 * Each phase is associated with a unique code and description.
 * The enum provides methods to retrieve the phase based on its code.
 */
public enum UpdatePhase {

    IDLE(0x00, "Ready to start a Receive Firmware procedure"),

    TRANSFER_ERROR(0x01, "The Transfer BLOB procedure failed."),

    TRANSFER_ACTIVE(0x02, "The Receive Firmware procedure is being executed"),

    VERIFICATION_ACTIVE(0x03, "The Verify Firmware procedure is being executed"),

    VERIFICATION_SUCCESS(0x04, "The Verify Firmware procedure completed successfully."),

    VERIFICATION_FAILED(0x05, "The Verify Firmware procedure failed."),

    APPLYING_UPDATE(0x06, "The Apply New Firmware procedure is being executed."),

    UNKNOWN_ERROR(0xFF, "unknown error");
    /**
     * The code associated with the status.
     */
    public final int code;
    /**
     * The description of the status.
     */
    public final String desc;

    /**
     * Constructs a new UpdatePhase with the given code and description.
     *
     * @param code The code associated with the status.
     * @param desc The description of the status.
     */
    UpdatePhase(int code, String desc) {
        this.code = code;
        this.desc = desc;
    }

    /**
     * Returns the UpdatePhase associated with the given code.
     * If no matching phase is found, returns UNKNOWN_ERROR.
     *
     * @param code The code to lookup.
     * @return The UpdatePhase associated with the code, or UNKNOWN_ERROR if not found.
     */
    public static UpdatePhase valueOf(int code) {
        for (UpdatePhase status : values()) {
            if (status.code == code) return status;
        }
        return UNKNOWN_ERROR;
    }
}
