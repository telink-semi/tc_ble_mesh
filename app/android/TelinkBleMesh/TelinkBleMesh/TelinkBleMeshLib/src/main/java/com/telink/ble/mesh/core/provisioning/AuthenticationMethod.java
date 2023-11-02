/********************************************************************************************************
 * @file AuthenticationMethod.java
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
package com.telink.ble.mesh.core.provisioning;


/**
 * This enum represents the available authentication methods for a device.
 * It includes NoOOB and StaticOOB.
 * NoOOB has a value of 0x00, indicating that no out-of-band (OOB) authentication is used.
 * StaticOOB has a value of 0x01, indicating that static OOB authentication is used.
 * Output OOB and input OOB are not supported by this enum.
 */
public enum AuthenticationMethod {
    NoOOB((byte) 0x00),
    StaticOOB((byte) 0x01);

    // output oob and input oob are not supported

    public final byte value;

    /**
     * constructor
     *
     * @param value value
     */
    AuthenticationMethod(byte value) {
        this.value = value;
    }
}
