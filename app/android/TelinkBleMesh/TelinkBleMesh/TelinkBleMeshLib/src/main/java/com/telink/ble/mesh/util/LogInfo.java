/********************************************************************************************************
 * @file LogInfo.java
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
package com.telink.ble.mesh.util;


import java.io.Serializable;

/**
 * Created by kee on 2019/1/11.
 */

public class LogInfo implements Serializable {
    public String tag = "";
    public long millis = 0;
    public int level;
    public String logMessage;
    public long threadId;
    public String threadName;

    public LogInfo(String tag, String logMessage, int level) {
        this.tag = tag;
        this.level = level;
        this.logMessage = logMessage;
        this.millis = System.currentTimeMillis();
        this.threadId = Thread.currentThread().getId();
        this.threadName = Thread.currentThread().getName();
    }


}
