/********************************************************************************************************
 * @file OTSUUID.java
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

import java.util.UUID;

public class OTSUUID {

    // service
    public static final UUID OBJECT_TRANSFER = UUID.fromString("1825");


    // characteristic
    public static final UUID OTS_Feature = UUID.fromString("2ABD");
    public static final UUID OBJECT_NAME = UUID.fromString("2ABE");
    public static final UUID OBJECT_TYPE = UUID.fromString("2ABF");
    public static final UUID OBJECT_SIZE = UUID.fromString("2AC0)");
    public static final UUID OBJECT_FIRST_CREATED = UUID.fromString("2AC1)");


    public static final UUID OBJECT_LAST_MODIFIED = UUID.fromString("2AC2)");

    public static final UUID OBJECT_ID = UUID.fromString("2AC3)");

    public static final UUID OBJECT_PROPERTIES = UUID.fromString("2AC4)");

    /**
     * OACP
     */
    public static final UUID OBJECT_ACTION_CONTROL_POINT = UUID.fromString("2AC5)");

    /**
     * OLCP
     */
    public static final UUID OBJECT_LIST_CONTROL_POINT = UUID.fromString("2AC6)");

    public static final UUID OBJECT_LIST_FILTER = UUID.fromString("2AC7)");


    public static final UUID OBJECT_changed = UUID.fromString("2AC8)");


}
