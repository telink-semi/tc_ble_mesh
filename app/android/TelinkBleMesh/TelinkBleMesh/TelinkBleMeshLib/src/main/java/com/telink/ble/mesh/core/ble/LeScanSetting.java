/********************************************************************************************************
 * @file LeScanSetting.java
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
package com.telink.ble.mesh.core.ble;

/**
 * Created by kee on 2019/9/9.
 */

public class LeScanSetting {

    /**
     * time between last scanning start time
     */
    public long spacing;

    /**
     * time of scanning
     */
    public long timeout;

    public static LeScanSetting getDefault() {
        LeScanSetting setting = new LeScanSetting();
        setting.spacing = 5 * 1000;
        setting.timeout = 10 * 1000;
        return setting;
    }

    public LeScanSetting() {
    }

    public LeScanSetting(long spacing, long during) {
        this.spacing = spacing;
        this.timeout = during;
    }
}
