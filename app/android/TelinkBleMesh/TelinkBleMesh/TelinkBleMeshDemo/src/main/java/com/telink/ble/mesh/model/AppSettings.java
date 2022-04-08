/********************************************************************************************************
 * @file AppSettings.java
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

public abstract class AppSettings {
    /**
     * is online-status enabled
     */
    public static boolean ONLINE_STATUS_ENABLE = false;

    // draft feature
    public static final boolean DRAFT_FEATURES_ENABLE = false;


    public static final int PID_CT = 0x01;

    public static final int PID_HSL = 0x02;

    public static final int PID_PANEL = 0x07;

    public static final int PID_LPN = 0x0201;

    public static final int PID_REMOTE = 0x0301;

}
