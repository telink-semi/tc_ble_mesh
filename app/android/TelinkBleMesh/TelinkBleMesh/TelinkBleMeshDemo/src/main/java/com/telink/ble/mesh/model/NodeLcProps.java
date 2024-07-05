/********************************************************************************************************
 * @file NodeLcProps.java
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

import com.telink.ble.mesh.core.DeviceProperty;

import java.io.Serializable;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;

/**
 * Created by kee on 2017/8/18.
 */

@Entity
public class NodeLcProps implements Serializable {
    @Id
    public long id;

//    public boolean enabled;

    /**
     * @see DeviceProperty#LC_LIGHTNESS_ON
     */
    public int lightnessOn = -1;

    /**
     * @see DeviceProperty#LIGHTNESS_PROLONG
     */
    public int lightnessProlong = -1;

    /**
     * @see DeviceProperty#LIGHTNESS_STANDBY
     */
    public int lightnessStandby = -1;

    /**
     * @see DeviceProperty#LC_AMBIENT_LUX_LEVEL_ON
     */
    public int luxLevelOn = -1;

    /**
     * @see DeviceProperty#LC_AMBIENT_LUX_LEVEL_PROLONG
     */
    public int luxLevelProlong = -1;

    /**
     * @see DeviceProperty#LC_AMBIENT_LUX_LEVEL_STANDBY
     */
    public int luxLevelStandby = -1;

    /**
     * @see DeviceProperty#TIME_FADE
     */
    public int timeFade = -1;

    /**
     * @see DeviceProperty#TIME_FADE_ON
     */
    public int timeFadeOn = -1;

    /**
     * @see DeviceProperty#TIME_FADE_STANDBY_AUTO
     */
    public int timeFadeStandbyAuto = -1;

    /**
     * @see DeviceProperty#TIME_FADE_STANDBY_MANUAL
     */
    public int timeFadeStandbyManual = -1;

    /**
     * @see DeviceProperty#TIME_OCCUPANCY_DELAY
     */
    public int timeOccupancyDelay = -1;

    /**
     * @see DeviceProperty#TIME_PROLONG
     */
    public int timeProlong = -1;

    /**
     * @see DeviceProperty#TIME_RUN_ON
     */
    public int timeRunOn = -1;

    /**
     * @see DeviceProperty#LC_REGULATOR_ACCURACY
     */
    public int regulatorAccuracy = -1;

    /**
     * @see DeviceProperty#LC_REGULATOR_KID
     */
    public int regulatorKid = -1;

    /**
     * @see DeviceProperty#LC_REGULATOR_KIU
     */
    public int regulatorKiu = -1;

    /**
     * @see DeviceProperty#LC_REGULATOR_KPD
     */
    public int regulatorKpd = -1;

    /**
     * @see DeviceProperty#LC_REGULATOR_KPU
     */
    public int regulatorKpu = -1;

    public void updatePropertyValue(int propertyId, int newVal) {
        DeviceProperty property = DeviceProperty.getById(propertyId);
        if (property == null) {
            return;
        }
        updatePropertyValue(property, newVal);
    }

    public void updatePropertyValue(DeviceProperty property, int newVal) {
        switch (property) {
            case LC_LIGHTNESS_ON:
                lightnessOn = newVal;
                break;

            case LIGHTNESS_PROLONG:
                lightnessProlong = newVal;
                break;

            case LIGHTNESS_STANDBY:
                lightnessStandby = newVal;
                break;

            case LC_AMBIENT_LUX_LEVEL_ON:
                luxLevelOn = newVal;
                break;

            case LC_AMBIENT_LUX_LEVEL_PROLONG:
                luxLevelProlong = newVal;
                break;

            case LC_AMBIENT_LUX_LEVEL_STANDBY:
                luxLevelStandby = newVal;
                break;

            case TIME_FADE:
                timeFade = newVal;
                break;

            case TIME_FADE_ON:
                timeFadeOn = newVal;
                break;

            case TIME_FADE_STANDBY_AUTO:
                timeFadeStandbyAuto = newVal;
                break;

            case TIME_FADE_STANDBY_MANUAL:
                timeFadeStandbyManual = newVal;
                break;

            case TIME_OCCUPANCY_DELAY:
                timeOccupancyDelay = newVal;
                break;

            case TIME_PROLONG:
                timeProlong = newVal;
                break;

            case TIME_RUN_ON:
                timeRunOn = newVal;
                break;

            case LC_REGULATOR_ACCURACY:
                regulatorAccuracy = newVal;
                break;

            case LC_REGULATOR_KID:
                regulatorKid = newVal;
                break;

            case LC_REGULATOR_KIU:
                regulatorKiu = newVal;
                break;
            case LC_REGULATOR_KPD:
                regulatorKpd = newVal;
                break;

            case LC_REGULATOR_KPU:
                regulatorKpu = newVal;
                break;
        }
    }

    public int getPropertyValue(int propertyId) {
        DeviceProperty property = DeviceProperty.getById(propertyId);
        if (property == null) {
            return 0;
        }
        return getPropertyValue(property);
    }


    public int getPropertyValue(DeviceProperty property) {
        switch (property) {
            case LC_LIGHTNESS_ON:
                return lightnessOn;

            case LIGHTNESS_PROLONG:
                return lightnessProlong;

            case LIGHTNESS_STANDBY:
                return lightnessStandby;

            case LC_AMBIENT_LUX_LEVEL_ON:
                return luxLevelOn;

            case LC_AMBIENT_LUX_LEVEL_PROLONG:
                return luxLevelProlong;

            case LC_AMBIENT_LUX_LEVEL_STANDBY:
                return luxLevelStandby;


            case TIME_FADE:
                return timeFade;

            case TIME_FADE_ON:
                return timeFadeOn;

            case TIME_FADE_STANDBY_AUTO:
                return timeFadeStandbyAuto;

            case TIME_FADE_STANDBY_MANUAL:
                return timeFadeStandbyManual;

            case TIME_OCCUPANCY_DELAY:
                return timeOccupancyDelay;

            case TIME_PROLONG:
                return timeProlong;

            case TIME_RUN_ON:
                return timeRunOn;

            case LC_REGULATOR_ACCURACY:
                return regulatorAccuracy;

            case LC_REGULATOR_KID:
                return regulatorKid;

            case LC_REGULATOR_KIU:
                return regulatorKiu;

            case LC_REGULATOR_KPD:
                return regulatorKpd;

            case LC_REGULATOR_KPU:
                return regulatorKpu;
        }
        return 0;
    }
}
