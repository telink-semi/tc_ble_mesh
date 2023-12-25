/********************************************************************************************************
 * @file BridgingTable.java
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
package com.telink.ble.mesh.model;

import java.io.Serializable;

/**
 * bridging table format
 */
public class BridgingTable implements Serializable {
    /**
     * Allowed directions for the bridged traffic
     * 8 bits
     */
    public byte directions;

    /**
     * NetKey Index of the first subnet
     * 12 bits
     */
    public int netKeyIndex1;

    /**
     * NetKey Index of the second subnet
     * 12 bits
     */
    public int netKeyIndex2;

    /**
     * Address of the node in the first subnet
     * 16 bits
     */
    public int address1;

    /**
     * Address of the node in the second subnet
     * 16 bits
     */
    public int address2;

    public enum Direction {

        // unidirectional
        UNIDIRECTIONAL(1, "UNIDIRECTIONAL"),
        // bidirectional
        BIDIRECTIONAL(2, "BIDIRECTIONAL");

        public final int value;
        public final String desc;

        Direction(int value, String desc) {
            this.value = value;
            this.desc = desc;
        }

        public static Direction getByValue(int value) {
            for (Direction direction :
                    values()) {
                if (direction.value == value) {
                    return direction;
                }
            }
            return UNIDIRECTIONAL;
        }
    }
}
