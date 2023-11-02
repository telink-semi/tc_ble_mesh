/********************************************************************************************************
 * @file DistributionPhase.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

/**
 * This enum represents the different phases of a firmware update process.
 */
public enum DistributionPhase {
    IDLE(0x00,
            "No firmware distribution is in progress."
    ),

    TRANSFER_ACTIVE(0x01,
            "Firmware distribution is in progress."
    ),

    TRANSFER_SUCCESS(0x02,
            "The Transfer BLOB procedure has completed successfully."
    ),

    APPLYING_UPDATE(0x03,
            "The Apply Firmware on Updating Nodes procedure is being executed."
    ),

    COMPLETED(0x04,
            "The Distribute Firmware procedure has completed successfully."
    ),

    FAILED(0x05,
            "The Distribute Firmware procedure has failed."
    ),

    CANCELING_UPDATE(0x06,
            "The Cancel Firmware Update procedure is being executed."
    ),
    ;

    public final int value;

    public final String desc;

    /**
     * Constructor for the DistributionPhase enum.
     *
     * @param value The numerical value representing the distribution phase.
     * @param desc  A description of the distribution phase.
     */
    DistributionPhase(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
