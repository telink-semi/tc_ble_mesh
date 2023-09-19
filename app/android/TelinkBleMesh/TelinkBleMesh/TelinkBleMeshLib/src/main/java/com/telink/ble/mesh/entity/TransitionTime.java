/********************************************************************************************************
 * @file TransitionTime.java
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
package com.telink.ble.mesh.entity;

/**
 * Created by kee on 2019/2/25.
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

    public TransitionTime(byte number, byte step) {
        this.number = number;
        this.step = step;
    }

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

    public byte getValue() {
        return (byte) ((step << 6) | number);
    }

    public byte getNumber() {
        return number;
    }

    public byte getStep() {
        return step;
    }

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
}
