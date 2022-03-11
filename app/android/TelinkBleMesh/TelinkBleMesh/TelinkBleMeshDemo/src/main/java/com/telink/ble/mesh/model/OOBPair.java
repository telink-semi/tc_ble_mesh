/********************************************************************************************************
 * @file OOBPair.java
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

public class OOBPair implements Serializable {
    /**
     * manual input in OOBEditActivity
     */
    public static final int IMPORT_MODE_MANUAL = 0;

    /**
     * batch import from valid formatted file
     */
    public static final int IMPORT_MODE_FILE = 1;

    /**
     * device UUID
     */
    public byte[] deviceUUID;

    /**
     * OOB value, used when device is static-oob supported
     */
    public byte[] oob;

    /**
     * @see #IMPORT_MODE_FILE
     * @see #IMPORT_MODE_MANUAL
     */
    public int importMode;

    /**
     * import time
     */
    public long timestamp;
}
