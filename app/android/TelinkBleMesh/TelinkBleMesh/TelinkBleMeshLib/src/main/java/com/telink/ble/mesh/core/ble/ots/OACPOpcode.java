/********************************************************************************************************
 * @file OACPOpcode.java
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
package com.telink.ble.mesh.core.ble.ots;

/**
 * OACP opcode
 */
public enum OACPOpcode {
    /**
     * Create
     * params: Size (UINT32), Type (gatt_uuid)
     */
    CREATE(0x01),

    /**
     * Delete
     * params: none
     */
    DELETE(0x02),


    /**
     * Calculate Checksum
     * params: Offset (UINT32), Length (UINT32)
     */
    CALCULATE_CHECKSUM(0x03),


    /**
     * Parameter may be defined by a higher-level spec;none otherwise.
     */
    Execute(0x04),


    /**
     * Offset (UINT32), Length (UINT32)
     */
    READ(0x05),

    /**
     * Write
     * Offset (UINT32), Length (UINT32), Mode (8bit)
     */
    WRITE(0x06),

    /**
     * Abort
     * no params
     */
    ABORT(0x07),

    /**
     * response code
     * params: OACP Response Value
     */
    RESPONSE(0x60);

    int code;

    OACPOpcode(int code) {
        this.code = code;
    }

}
