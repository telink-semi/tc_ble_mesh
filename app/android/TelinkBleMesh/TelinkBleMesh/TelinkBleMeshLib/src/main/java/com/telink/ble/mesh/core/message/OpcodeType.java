/********************************************************************************************************
 * @file OpcodeType.java
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
package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.MeshUtils;

public enum OpcodeType {
    SIG_1(1),
    SIG_2(2),
    VENDOR(3);

    public final int length;

    OpcodeType(int length) {
        this.length = length;
    }

    /**
     * @param opFst first byte of opcode
     */
    public static OpcodeType getByFirstByte(byte opFst) {
        return (opFst & MeshUtils.bit(7)) != 0
                ?
                ((opFst & MeshUtils.bit(6)) != 0 ? VENDOR : SIG_2)
                :
                SIG_1;
    }
}
