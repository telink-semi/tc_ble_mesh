/********************************************************************************************************
 * @file TransitionTime.java
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
package com.telink.ble.mesh.entity;

/**
 * The TransitionTime class represents a transition time value used in a system.
 * It consists of a number and a step, which together determine the resolution and duration of the transition.
 * The number is a value between 0 and 63, representing the number of steps in the transition.
 * The step is a 2-bit value, indicating the resolution of the transition.
 * The class provides methods for creating a TransitionTime object from a given time in milliseconds,
 * as well as retrieving the value, number, step, and resolution of a TransitionTime object.
 */
public class TransitionTime {
    // 6 bits
    private byte number;
    /**
     * 2 bits
     * 0b00
     * The Default Transition Step Resolution is 100 milliseconds
     * 0b01
     * The Default Transition Step Resolution is 1 second
     * 0b10
     * The Default Transition Step Resolution is 10 seconds
     * 0b11
     * The Default Transition Step Resolution is 10 minutes
     */
    private byte step;
    // 0b111111
    private static final int MAX_STEP_VALUE = 0x3F;
    private static final byte STEP_RESOLUTION_100_MILL = 0b00;
    private static final byte STEP_RESOLUTION_1_SECOND = 0b01;
    private static final byte STEP_RESOLUTION_10_SECOND = 0b10;
    private static final byte STEP_RESOLUTION_10_MINUTE = 0b11;
    private static final int PERIOD_STEP_100_MILL = 100;
    private static final int PERIOD_STEP_1_SECOND = 1000;
    private static final int PERIOD_STEP_10_SECOND = 10 * 1000;
    private static final int PERIOD_STEP_10_MINUTE = 10 * 60 * 1000;

    /**
     * Constructs a TransitionTime object with the given number and step values.
     *
     * @param number the number of steps in the transition
     * @param step   the resolution of the transition
     */
    public TransitionTime(byte number, byte step) {
        this.number = number;
        this.step = step;
    }

    /**
     * Creates a TransitionTime object from the given time in milliseconds.
     *
     * @param millisecond the time in milliseconds
     * @return a TransitionTime object representing the given time
     */
    public static TransitionTime fromTime(long millisecond) {
        byte step = 0, number = 0;
        if (millisecond <= 0) {
            step = 0;
            number = 0;
        } else if (millisecond <= MAX_STEP_VALUE * PERIOD_STEP_100_MILL) {
            step = STEP_RESOLUTION_100_MILL;
            number = (byte) (millisecond / PERIOD_STEP_100_MILL);
        } else if (millisecond <= MAX_STEP_VALUE * PERIOD_STEP_1_SECOND) {
            step = STEP_RESOLUTION_1_SECOND;
            number = (byte) (millisecond / PERIOD_STEP_1_SECOND);
        } else if (millisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_SECOND) {
            step = STEP_RESOLUTION_10_SECOND;
            number = (byte) (millisecond / PERIOD_STEP_10_SECOND);
        } else if (millisecond <= MAX_STEP_VALUE * PERIOD_STEP_10_MINUTE) {
            step = STEP_RESOLUTION_10_MINUTE;
            number = (byte) (millisecond / PERIOD_STEP_10_MINUTE);
        }
        return new TransitionTime(number, step);
    }

    /**
     * Returns the value of the TransitionTime object.
     * The value is a byte representation of the number and step values.
     *
     * @return the value of the TransitionTime object
     */
    public byte getValue() {
        return (byte) ((step << 6) | number);
    }

    /**
     * Returns the number of steps in the transition.
     *
     * @return the number of steps
     */
    public byte getNumber() {
        return number;
    }

    /**
     * Returns the resolution of the transition.
     *
     * @return the resolution in milliseconds
     */
    public byte getStep() {
        return step;
    }


    /**
     * Returns the resolution of the transition in milliseconds.
     *
     * @return the resolution in milliseconds
     */
    public int getResolution() {
        switch (step) {
            case STEP_RESOLUTION_100_MILL:
                return PERIOD_STEP_100_MILL;
            case STEP_RESOLUTION_1_SECOND:
                return PERIOD_STEP_1_SECOND;
            case STEP_RESOLUTION_10_SECOND:
                return PERIOD_STEP_10_SECOND;
            case STEP_RESOLUTION_10_MINUTE:
                return PERIOD_STEP_10_MINUTE;
        }
        return 0;
    }

    /**
     * Returns the resolution of the transition in milliseconds.
     *
     * @return the resolution in milliseconds
     */
    public static int getResolution(int step) {
        switch (step) {
            case STEP_RESOLUTION_100_MILL:
                return PERIOD_STEP_100_MILL;
            case STEP_RESOLUTION_1_SECOND:
                return PERIOD_STEP_1_SECOND;
            case STEP_RESOLUTION_10_SECOND:
                return PERIOD_STEP_10_SECOND;
            case STEP_RESOLUTION_10_MINUTE:
                return PERIOD_STEP_10_MINUTE;
        }
        return 0;
    }

    public static int getTimeMilliseconds(int value) {
        int st = value >> 6;
        int no = value & 0x3F;
        return getResolution(st) * no;
    }
}