/********************************************************************************************************
 * @file SchedulerAction.java
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
package com.telink.ble.mesh.model.web;

/**
 * scheduler action
 */
public enum SchedulerAction {


    OFF(0, "off"),

    ON(1, "on"),

    /**
     * 允许以成员角色访问， 允许组网和发送配置指令
     */
    SCENE(2, "scene"),

    NO(3, "no"),
    ;
    public final long action;
    public final String desc;

    SchedulerAction(long action, String desc) {
        this.action = action;
        this.desc = desc;
    }

    public static SchedulerAction getByAction(long action) {
        for (SchedulerAction st : values()) {
            if (st.action == action) {
                return st;
            }
        }
        return null;
    }

}
