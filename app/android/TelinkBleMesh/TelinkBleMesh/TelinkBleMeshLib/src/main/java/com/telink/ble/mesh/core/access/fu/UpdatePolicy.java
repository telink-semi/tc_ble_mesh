/********************************************************************************************************
 * @file UpdatePolicy.java
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
package com.telink.ble.mesh.core.access.fu;

import java.io.Serializable;

public enum UpdatePolicy implements Serializable {
    VerifyOnly(0,
            "The Firmware Distribution Server verifies that firmware image distribution completed successfully but does not apply the update. The Initiator (the Firmware Distribution Client) initiates firmware image application"
    ),
    VerifyAndApply(1,
            "The Firmware Distribution Server verifies that firmware image distribution completed successfully and then applies the firmware update"
    );

    final int value;
    final String desc;

    UpdatePolicy(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
