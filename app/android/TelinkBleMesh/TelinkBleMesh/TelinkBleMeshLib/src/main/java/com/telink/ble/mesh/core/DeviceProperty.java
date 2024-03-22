/********************************************************************************************************
 * @file DeviceProperty.java
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
package com.telink.ble.mesh.core;

import java.io.Serializable;

/**
 * device property
 * see Device
 */
public enum DeviceProperty implements Serializable {


    LC_AMBIENT_LUX_LEVEL_ON(0x002B, 3, "Ambient Lux Level On", "Illuminance",
            "This property represents the minimum ambient illuminance level as measured by a lux sensor that determines if a light or a group of lights transitions from the standby state to a run state. "),

    LC_AMBIENT_LUX_LEVEL_PROLONG(0x002C, 3, "Ambient Lux Level Prolong", "Illuminance",
                    "This property represents the ambient light level for a light or a group of lights in the prolong state."),

    LC_AMBIENT_LUX_LEVEL_STANDBY(0x002D, 3, "Ambient Lux Level Standby", "Illuminance",
            "This property represents the ambient light level for a light or a group of lights to remain in a standby state."),

    LC_REGULATOR_ACCURACY(0x0031, 1, "Regulator Accuracy", "Percentage 8",
                    "This property represents the accuracy of a proportional-integral light regulator. "),

    LC_REGULATOR_KID(0x0032, 1, "Regulator Kid", "Percentage 8",
            "This property represents the integral coefficient K_i in a decreasing output operation mode of a proportional-integral light controller."),

    LC_REGULATOR_KIU(0x0033, 1, "Regulator Kiu", "Percentage 8",
            "This property represents the integral coefficient K_i in an increasing output operation mode of a proportional-integral light controller. "),

    LC_REGULATOR_KPD(0x0034, 1, "Regulator Kpd", "Percentage 8",
                    "This property represents the proportional coefficient K_p in a decreasing output operation mode of a proportional-integral light controller."),

    LC_REGULATOR_KPU(0x0035, 1, "Regulator Kpu", "Percentage 8",
            "This property represents the proportional coefficient K_p in an increasing output operation mode of a proportional-integral light controller. "),



    LC_LIGHTNESS_ON(0x002E, 2, "Lightness On", "Perceived Lightness",
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
            "This property represents the duration of the run state after last occupancy was detected."),


    // sensor

    MOTION_SENSED(0x0042, 1, "Motion Sensed", "Percentage 8",
            "This property represents the activity level, as, for example, detected by a motion sensor."),

//    PRESENT_AMBIENT_LIGHT_LEVEL(0x004E, 3, "Present Ambient Light Level", "Illuminance",
//            "This property represents the light level as measured by a light sensor measuring illuminance (Lux)."),
    PRESENT_AMBIENT_LIGHT_LEVEL(0x004E, 3, "Present Ambient", "Illuminance",
            "This property represents the light level as measured by a light sensor measuring illuminance (Lux)."),

    PRESENCE_DETECTED(0x004D, 1, "Presence Detected", "Boolean",
            "This property represents whether or not an occupant is detected within range of the occupancy detector."),

    SENSOR_GAIN(0x0074, 1, "Sensor Gain", "Coefficient",
            "This property represents the ratio of the value reported by the sensor to the raw value measured by the sensor."),

    MOTION_THRESHOLD(0x0043, 1, "Motion Threshold", "Percentage 8",
            "This property represents the activity level, for example, as detected by a motion or an occupancy sensor, below which motion or occupancy is not reported."),

    PEOPLE_COUNT(0x004C, 2, "People Count", "Count 16",
            "This property represents the number of people present in a space or in a building. "),

    TIME_SINCE_MOTION_SENSED(0x0068, 3, "Time Since Motion Sensed", "Time Millisecond 24",
            "This property represents the time that has elapsed since the sensor last detected any activity. "),

    PRECISE_TOTAL_DEVICE_ENERGY_USE(0x0072, 4, "Precise Total Device Energy Use", "Energy 32",
            "This property represents the total energy used by a device over its lifetime."),

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

    DeviceProperty(int id, int len, String name, String characteristic, String def) {
        this.id = id;
        this.len = len;
        this.name = name;
        this.characteristic = characteristic;
        this.def = def;
    }


    public static DeviceProperty[] getLcProperties() {
        return new DeviceProperty[]{
                LC_LIGHTNESS_ON,
                LIGHTNESS_PROLONG,
                LIGHTNESS_STANDBY,
                LC_AMBIENT_LUX_LEVEL_ON,
                LC_AMBIENT_LUX_LEVEL_PROLONG,
                LC_AMBIENT_LUX_LEVEL_STANDBY,
                TIME_FADE_ON,
                TIME_RUN_ON,
                TIME_FADE,
                TIME_PROLONG,
                TIME_FADE_STANDBY_AUTO,
                TIME_FADE_STANDBY_MANUAL,
                TIME_OCCUPANCY_DELAY,
                LC_REGULATOR_ACCURACY,
                LC_REGULATOR_KID,
                LC_REGULATOR_KIU,
                LC_REGULATOR_KPD,
                LC_REGULATOR_KPU,

        };
    }

    public static DeviceProperty getById(int id) {
        for (DeviceProperty property : values()) {
            if (property.id == id) return property;
        }
        return null;
    }

}
