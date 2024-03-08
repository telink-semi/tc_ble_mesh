/********************************************************************************************************
 * @file MeshSigModel.java
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

import java.io.Serializable;


/**
 * This enum represents the different mesh SIG models available in a mesh network.
 * Each model has a unique model ID, a model name, and a group description.
 * Some models use device key for encryption, while others use application key.
 */
public enum MeshSigModel implements Serializable {

    SIG_MD_CFG_SERVER(0x0000, "config server", "", true),
    SIG_MD_CFG_CLIENT(0x0001, "config client", "", true),


    SIG_MD_HEALTH_SERVER(0x0002, "health server", "health server", false),
    SIG_MD_HEALTH_CLIENT(0x0003, "health client", "health client", false),


    SIG_MD_REMOTE_PROV_SERVER(0x0004, "remote provision server", "", true),
    SIG_MD_REMOTE_PROV_CLIENT(0x0005, "remote provision client", "", true),

    SIG_MD_DF_CFG_S(0x0006, "Directed Forwarding Configuration server", "", true),
    SIG_MD_DF_CFG_C(0x0007, "Directed Forwarding Configuration client", "", true),

    SIG_MD_BRIDGE_CFG_SERVER(0x0008, "Bridge Configuration Server", "", true),
    SIG_MD_BRIDGE_CFG_CLIENT(0x0009, "Bridge Configuration Client", "", true),

    SIG_MD_PRIVATE_BEACON_SERVER(0x000A, "Mesh Private Beacon Server", "", true),
    SIG_MD_PRIVATE_BEACON_CLIENT(0x000B, "Mesh Private Beacon Client", "", true),

    SIG_MD_ON_DEMAND_PROXY_S(0x000C, "On-Demand Private Proxy Server", "", true),
    SIG_MD_ON_DEMAND_PROXY_C(0x000D, "On-Demand Private Proxy Client", "", true),

    SIG_MD_SAR_CFG_S(0x000E, "SAR Configuration Server", "", true),
    SIG_MD_SAR_CFG_C(0x000F, "SAR Configuration Client", "", true),

    // opcode aggregator
    SIG_MD_CFG_OP_AGG_S(0x0010, "Opcodes Aggregator Server", "aggregator", true),
    SIG_MD_CFG_OP_AGG_C(0x0011, "Opcodes Aggregator Client", "aggregator", true),

    // large composition data
    SIG_MD_LARGE_CPS_S(0x0012, "Large Composition Data Server", "", true),
    SIG_MD_LARGE_CPS_C(0x0013, "Large Composition Data Client", "", true),

    // Solicitation PDU RPL Configuration
    SIG_MD_SOL_PDU_RPL_CFG_S(0x0014, "Solicitation PDU RPL Configuration Server", "", true),
    SIG_MD_SOL_PDU_RPL_CFG_C(0x0015, "Solicitation PDU RPL Configuration Client", "", true),


    SIG_MD_G_ONOFF_S(0x1000, "Generic OnOff Server", "Generic"),
    SIG_MD_G_ONOFF_C(0x1001, "Generic OnOff Client", "Generic"),
    SIG_MD_G_LEVEL_S(0x1002, "Generic Level Server", "Generic"),
    SIG_MD_G_LEVEL_C(0x1003, "Generic Level Client", "Generic"),
    SIG_MD_G_DEF_TRANSIT_TIME_S(0x1004, "Generic Default Transition Time Server ", "Generic"),
    SIG_MD_G_DEF_TRANSIT_TIME_C(0x1005, "Generic Default Transition Time Client ", "Generic"),
    SIG_MD_G_POWER_ONOFF_S(0x1006, "Generic Power OnOff Server", "Generic"),
    SIG_MD_G_POWER_ONOFF_SETUP_S(0x1007, "Generic Power OnOff Setup Server", "Generic"),
    SIG_MD_G_POWER_ONOFF_C(0x1008, "Generic Power OnOff Client", "Generic"),
    SIG_MD_G_POWER_LEVEL_S(0x1009, "Generic Power Level Server", "Generic"),
    SIG_MD_G_POWER_LEVEL_SETUP_S(0x100A, "Generic Power Level Setup Server", "Generic"),
    SIG_MD_G_POWER_LEVEL_C(0x100B, "Generic Power Level Client", "Generic"),
    SIG_MD_G_BAT_S(0x100C, "Generic Battery Server", "Generic"),
    SIG_MD_G_BAT_C(0x100D, "Generic Battery Client", "Generic"),
    SIG_MD_G_LOCATION_S(0x100E, "Generic Location Server", "Generic"),
    SIG_MD_G_LOCATION_SETUP_S(0x100F, "Generic Location Setup Server", "Generic"),
    SIG_MD_G_LOCATION_C(0x1010, "Generic Location Client", "Generic"),
    SIG_MD_G_ADMIN_PROP_S(0x1011, "Generic Admin Property Server", "Generic"),
    SIG_MD_G_MFR_PROP_S(0x1012, "Generic Manufacturer Property Server", "Generic"),
    SIG_MD_G_USER_PROP_S(0x1013, "Generic User Property Server", "Generic"),
    SIG_MD_G_CLIENT_PROP_S(0x1014, "Generic Client Property Server", "Generic"),
    SIG_MD_G_PROP_C(0x1015, "Generic Property Client", "Generic"), // original: SIG_MD_G_PROP_S (may be err)

    SIG_MD_SENSOR_S(0x1100, "Sensor Server", "Sensors"),
    SIG_MD_SENSOR_SETUP_S(0x1101, "Sensor Setup Server", "Sensors"),
    SIG_MD_SENSOR_C(0x1102, "Sensor Client", "Sensors"),

    SIG_MD_TIME_S(0x1200, "Time Server", "Time and Scenes"),
    SIG_MD_TIME_SETUP_S(0x1201, "Time Setup Server", "Time and Scenes"),
    SIG_MD_TIME_C(0x1202, "Time Client", "Time and Scenes"),
    SIG_MD_SCENE_S(0x1203, "Scene Server", "Time and Scenes"),
    SIG_MD_SCENE_SETUP_S(0x1204, "Scene Setup Server", "Time and Scenes"),
    SIG_MD_SCENE_C(0x1205, "Scene Client", "Time and Scenes"),
    SIG_MD_SCHED_S(0x1206, "MeshScheduler Server", "Time and Scenes"),
    SIG_MD_SCHED_SETUP_S(0x1207, "MeshScheduler Setup Server", "Time and Scenes"),
    SIG_MD_SCHED_C(0x1208, "MeshScheduler Client", "Time and Scenes"),

    SIG_MD_LIGHTNESS_S(0x1300, "Light Lightness Server", "Lighting"),
    SIG_MD_LIGHTNESS_SETUP_S(0x1301, "Light Lightness Setup Server", "Lighting"),
    SIG_MD_LIGHTNESS_C(0x1302, "Light Lightness Client", "Lighting"),
    SIG_MD_LIGHT_CTL_S(0x1303, "Light CTL Server", "Lighting"),
    SIG_MD_LIGHT_CTL_SETUP_S(0x1304, "Light CTL Setup Server", "Lighting"),
    SIG_MD_LIGHT_CTL_C(0x1305, "Light CTL Client", "Lighting"),
    SIG_MD_LIGHT_CTL_TEMP_S(0x1306, "Light CTL Temperature Server", "Lighting"),
    SIG_MD_LIGHT_HSL_S(0x1307, "Light HSL Server", "Lighting"),
    SIG_MD_LIGHT_HSL_SETUP_S(0x1308, "Light HSL Setup Server", "Lighting"),
    SIG_MD_LIGHT_HSL_C(0x1309, "Light HSL Client", "Lighting"),
    SIG_MD_LIGHT_HSL_HUE_S(0x130A, "Light HSL Hue Server", "Lighting"),
    SIG_MD_LIGHT_HSL_SAT_S(0x130B, "Light HSL Saturation Server", "Lighting"),
    SIG_MD_LIGHT_XYL_S(0x130C, "Light xyL Server", "Lighting"),
    SIG_MD_LIGHT_XYL_SETUP_S(0x130D, "Light xyL Setup Server", "Lighting"),
    SIG_MD_LIGHT_XYL_C(0x130E, "Light xyL Client", "Lighting"),
    SIG_MD_LIGHT_LC_S(0x130F, "Light LC Server", "Lighting"),
    SIG_MD_LIGHT_LC_SETUP_S(0x1310, "Light LC Setup Server", "Lighting"),
    SIG_MD_LIGHT_LC_C(0x1311, "Light LC Client", "Lighting"),

//    SIG_MD_DF_CFG_S
//    SIG_MD_CFG_DF_S(0xBF30, "direct forwarding server", "", true),
//    SIG_MD_CFG_DF_C(0xBF31, "direct forwarding client", "", true),

//    SIG_MD_CFG_BRIDGE_S(0xBF32, "subnet bridge server", "", true),
//    SIG_MD_CFG_BRIDGE_C(0xBF33, "subnet bridge client", "", true),


    SIG_MD_OBJ_TRANSFER_S(0x1400, "BLOB Transfer Server", "OTA"),
    SIG_MD_OBJ_TRANSFER_C(0x1401, "BLOB Transfer Client", "OTA"),

    /**
     * firmware update model
     */
    SIG_MD_FW_UPDATE_S(0x1402, "Firmware Update Server", "OTA"),
    SIG_MD_FW_UPDATE_C(0x1403, "Firmware Update Client", "OTA"),
    SIG_MD_FW_DISTRIBUT_S(0x1404, "Firmware Distribute Server", "OTA"),
    SIG_MD_FW_DISTRIBUT_C(0x1405, "Firmware Distribute Client", "OTA"),
//    SIG_MD_OBJ_TRANSFER_S(0xFF00, "Object Transfer Server", "OTA"),
//    SIG_MD_OBJ_TRANSFER_C(0xFF01, "Object Transfer Client", "OTA"),


    ;

    /**
     * sig model id
     * 2 bytes
     */
    public int modelId;

    /**
     * model name
     */
    public String modelName;

    /**
     * model group desc
     */
    public String group;

    /**
     * use device key for encryption
     * otherwise use application key
     */
    public boolean deviceKeyEnc = false;

    public boolean selected;

    MeshSigModel(int modelId, String modelName, String group) {
        this(modelId, modelName, group, false);
    }

    MeshSigModel(int modelId, String modelName, String group, boolean deviceKeyEnc) {
        this.modelId = modelId;
        this.modelName = modelName;
        this.group = group;
        this.deviceKeyEnc = deviceKeyEnc;
    }

    public static boolean useDeviceKeyForEnc(int modelId) {
        MeshSigModel model = getById(modelId);
        return model != null && model.deviceKeyEnc;
    }

    public static boolean useDeviceKeyForEnc(String modelIdHex) {
        int modelId = Integer.valueOf(modelIdHex, 16);
        MeshSigModel model = getById(modelId);
        return model != null && model.deviceKeyEnc;
    }

    // default sub list
    public static MeshSigModel[] getDefaultSubList() {
        return new MeshSigModel[]{SIG_MD_G_ONOFF_S, SIG_MD_LIGHTNESS_S, SIG_MD_LIGHT_CTL_S,
                SIG_MD_LIGHT_CTL_TEMP_S, SIG_MD_LIGHT_HSL_S,
                SIG_MD_LIGHT_LC_S}; //  add SIG_MD_LIGHT_LC_S for lc support
    }

    public static MeshSigModel[] getLevelAssociatedList() {
        return new MeshSigModel[]{SIG_MD_LIGHTNESS_S, SIG_MD_LIGHT_CTL_TEMP_S, SIG_MD_LIGHT_HSL_HUE_S,
                SIG_MD_LIGHT_HSL_SAT_S};

    }

    public static MeshSigModel getById(int modelId) {
        for (MeshSigModel model :
                values()) {
            if (model.modelId == modelId) return model;
        }
        return null;
    }

}

