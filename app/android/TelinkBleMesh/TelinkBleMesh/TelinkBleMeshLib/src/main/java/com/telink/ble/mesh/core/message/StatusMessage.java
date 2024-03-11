/********************************************************************************************************
 * @file StatusMessage.java
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
package com.telink.ble.mesh.core.message;

import android.os.Parcelable;

import com.telink.ble.mesh.core.networking.AccessLayerPDU;

/**
 * status notification by acknowledged message
 * This is an abstract class for status notification messages.
 * Created by kee on 2019/8/20.
 */
public abstract class StatusMessage implements Parcelable {
    /**
     * Default constructor for the StatusMessage class.
     */
    public StatusMessage() {

    }

    /**
     * parse status message with access layer pdu params
     * This method should be implemented by subclasses to parse the status message using the provided access layer PDU params.
     * {@link AccessLayerPDU#params}
     *
     * @param params The parameters used to parse the status message.
     */
    public abstract void parse(byte[] params);


    /**
     * This method creates a new StatusMessage object based on the given opcode and parameters.
     * It checks if the opcode is registered in the MeshStatus.Container class and returns the corresponding message class.
     * If the message class is an instance of StatusMessage, it creates a new instance of that class, parses the parameters, and returns the status message.
     *
     * @param opcode The opcode of the message.
     * @param params The parameters used to create the status message.
     * @return The created status message object.
     * Returns null if the target opcode is not registered in the MeshStatus.Container class.
     * {@link MeshStatus.Container}
     */
    public static StatusMessage createByAccessMessage(int opcode, byte[] params) {

        Class messageClass = MeshStatus.Container.getMessageClass(opcode);
        if (messageClass != null) {
            Object msgClass = null;
            try {
                msgClass = messageClass.newInstance();
                if (msgClass instanceof StatusMessage) {
                    StatusMessage statusMessage = (StatusMessage) msgClass;
                    if (params != null) {
                        statusMessage.parse(params);
                    }
                    return statusMessage;
                }
            } catch (InstantiationException | IllegalAccessException | RuntimeException e) {
                e.printStackTrace();
            }

        }

        return null;
    }


}


/**
 * ALL status message opcode
 */
    /*

        Opcode messageOpcode = Opcode.valueOf(opcode);
        StatusMessage statusMessage = null;
        if (messageOpcode == null) return null;
        switch (messageOpcode) {
            case COMPOSITION_DATA_STATUS:
            case HEALTH_CURRENT_STATUS:
            case HEALTH_FAULT_STATUS:
            case HEARTBEAT_PUB_STATUS:
            case APPKEY_STATUS:
            case HEALTH_ATTENTION_STATUS:
            case CFG_BEACON_STATUS:
            case CFG_DEFAULT_TTL_STATUS:
            case CFG_FRIEND_STATUS:
            case CFG_GATT_PROXY_STATUS:
            case CFG_KEY_REFRESH_PHASE_STATUS:
            case CFG_MODEL_PUB_STATUS:
            case CFG_MODEL_SUB_STATUS:
            case CFG_NW_TRANSMIT_STATUS:
            case CFG_RELAY_STATUS:
            case CFG_LPN_POLL_TIMEOUT_STATUS:
            case HEALTH_PERIOD_STATUS:
            case HEARTBEAT_SUB_STATUS:
            case MODE_APP_STATUS:
            case NETKEY_STATUS:
            case NODE_ID_STATUS:
            case NODE_RESET_STATUS:

            case G_ONOFF_STATUS:
                statusMessage = new OnOffStatusMessage();
                break;
            case G_LEVEL_STATUS:
            case G_DEF_TRANS_TIME_STATUS:
            case G_ON_POWER_UP_STATUS:
            case G_POWER_LEVEL_STATUS:
            case G_POWER_LEVEL_LAST_STATUS:
            case G_POWER_DEF_STATUS:
            case G_POWER_LEVEL_RANGE_STATUS:
            case G_BATTERY_STATUS:
            case G_LOCATION_GLOBAL_STATUS:
            case G_LOCATION_LOCAL_STATUS:
            case LIGHTNESS_STATUS:
            case LIGHTNESS_LINEAR_STATUS:
            case LIGHTNESS_LAST_STATUS:
            case LIGHTNESS_DEFULT_STATUS:
            case LIGHTNESS_RANGE_STATUS:
            case LIGHT_CTL_STATUS:
            case LIGHT_CTL_TEMP_RANGE_STATUS:
            case LIGHT_CTL_TEMP_STATUS:
            case LIGHT_CTL_DEFULT_STATUS:
            case LIGHT_HSL_HUE_STATUS:
            case LIGHT_HSL_SAT_STATUS:
            case LIGHT_HSL_STATUS:
            case LIGHT_HSL_TARGET_STATUS:
            case LIGHT_HSL_DEF_STATUS:
            case LIGHT_HSL_RANGE_STATUS:
            case TIME_STATUS:
            case TIME_ROLE_STATUS:
            case TIME_ZONE_STATUS:
            case TAI_UTC_DELTA_STATUS:
            case SCHD_ACTION_STATUS:
            case SCHD_STATUS:
            case SCENE_STATUS:
            case SCENE_REG_STATUS:


                break;
        }

     */