/********************************************************************************************************
 * @file LightControlProperty.java
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
 * light control property
 */
public enum LightControlProperty implements Serializable {

    LIGHTNESS_ON(0x002E, 2, "Lightness On", "Perceived Lightness",
            "This property represents the light lightness level of a light or a group of lights in a run state."),

    LIGHTNESS_PROLONG(0x002F, 2, "Lightness Prolong", "Perceived Lightness",
            "This property represents the light lightness level of a light or a group of lights when in a prolong state."),

    LIGHTNESS_STANDBY(0x0030, 2, "Lightness Standby", "Perceived Lightness",
            "This property represents the perceived light lightness level of a light or a group of lights when in a standby state."),

    TIME_FADE(0x0036, 3, "Time Fade", "Time Millisecond 24",
            "This property represents the time a light takes to transition from a run state to a prolong state. The run state is the state when the light is running at normal light level, the prolong state is an intermediate state of a light between the run state and the standby state."),

    TIME_FADE_ON(0x0037, 3, "Time Fade On", "Time Millisecond 24",
            "This property represents the time lights take to transition from a standby state to a run state."),

    TIME_FADE_STANDBY_AUTO(0x0038, 3, "Time Fade Standby Auto", "Time Millisecond 24",
            "This property represents the time lights transition from a prolong state to a standby state when the transition is automatic (such as when triggered by an occupancy or light sensor)."),

    TIME_FADE_STANDBY_MANUAL(0x0039, 3, "Time Fade Standby Manual", "Time Millisecond 24",
            "This property represents the time lights take to transition to a standby state when the transition is triggered by a manual operation (e.g., by a user operating a light switch)."),

    TIME_OCCUPANCY_DELAY(0x003A, 3, "Time Occupancy Delay", "Time Millisecond 24",
            "This property represents the time delay between receiving a signal from an occupancy sensor and a light controller executing a state change as a result of the signal."),

    TIME_PROLONG(0x003B, 3, "Time Prolong", "Time Millisecond 24",
            "This property represents the duration of the prolong state, which is the state of a device between its run state and its standby state."),

    TIME_RUN_ON(0x003C, 3, "Time Run On", "Time Millisecond 24",
            "This property represents the duration of the run state after last occupancy was detected.\n"),
    ;
    /**
     * property id
     * defined in Device Property spec
     */
    public final int id;

    /**
     * byte length
     */
    public final int len;

    public final String name;

    public final String characteristic;

    /**
     * definition of the property
     */
    public final String def;

    LightControlProperty(int id, int len, String name, String characteristic, String def) {
        this.id = id;
        this.len = len;
        this.name = name;
        this.characteristic = characteristic;
        this.def = def;
    }

    public static LightControlProperty getById(int id) {
        for (LightControlProperty property : values()) {
            if (property.id == id) return property;
        }
        return null;
    }
}
