/********************************************************************************************************
 * @file UploadPhase.java
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
 * upload phase
 */
public enum UploadPhase {

    IDLE(0x00,
            "No firmware upload is in progress."
    ),

    TRANSFER_ACTIVE(0x01,
            "The Store Firmware or Store Firmware OOB  procedure is being executed."
    ),

    TRANSFER_ERROR(0x02,
            "The Store Firmware procedure or Store Firmware OOB procedure failed."
    ),

    TRANSFER_SUCCESS(0x03,
            "The Store Firmware procedure or the Store Firmware OOB procedure completed successfully."
    ),

    ;

    public final int value;

    public final String desc;

    UploadPhase(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
