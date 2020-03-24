/********************************************************************************************************
 * @file     SigMeshModel.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.sig.mesh.model;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by kee on 2018/6/6.
 */

public enum SigMeshModel implements Serializable {

    SIG_MD_CFG_SERVER((short)0x0000, "", ""),
    SIG_MD_CFG_CLIENT((short)0x0001, "", ""),
    SIG_MD_HEALTH_SERVER((short) 0x0002, "health server", "health server"),

    // out of default
    SIG_MD_HEALTH_CLIENT((short) 0x0003, "health client", "health client", false),

    SIG_MD_G_ONOFF_S((short) 0x1000, "Generic OnOff Server", "Generic"),
    SIG_MD_G_ONOFF_C((short) 0x1001, "Generic OnOff Client", "Generic"),
    SIG_MD_G_LEVEL_S((short) 0x1002, "Generic Level Server", "Generic"),
    SIG_MD_G_LEVEL_C((short) 0x1003, "Generic Level Client", "Generic"),
    SIG_MD_G_DEF_TRANSIT_TIME_S((short) 0x1004, "Generic Default Transition Time Server ", "Generic"),
    SIG_MD_G_DEF_TRANSIT_TIME_C((short) 0x1005, "Generic Default Transition Time Client ", "Generic"),
    SIG_MD_G_POWER_ONOFF_S((short) 0x1006, "Generic Power OnOff Server", "Generic"),
    SIG_MD_G_POWER_ONOFF_SETUP_S((short) 0x1007, "Generic Power OnOff Setup Server", "Generic"),
    SIG_MD_G_POWER_ONOFF_C((short) 0x1008, "Generic Power OnOff Client", "Generic"),
    SIG_MD_G_POWER_LEVEL_S((short) 0x1009, "Generic Power Level Server", "Generic"),
    SIG_MD_G_POWER_LEVEL_SETUP_S((short) 0x100A, "Generic Power Level Setup Server", "Generic"),
    SIG_MD_G_POWER_LEVEL_C((short) 0x100B, "Generic Power Level Client", "Generic"),
    SIG_MD_G_BAT_S((short) 0x100C, "Generic Battery Server", "Generic"),
    SIG_MD_G_BAT_C((short) 0x100D, "Generic Battery Client", "Generic"),
    SIG_MD_G_LOCATION_S((short) 0x100E, "Generic Location Server", "Generic"),
    SIG_MD_G_LOCATION_SETUP_S((short) 0x100F, "Generic Location Setup Server", "Generic"),
    SIG_MD_G_LOCATION_C((short) 0x1010, "Generic Location Client", "Generic"),
    SIG_MD_G_ADMIN_PROP_S((short) 0x1011, "Generic Admin Property Server", "Generic"),
    SIG_MD_G_MFR_PROP_S((short) 0x1012, "Generic Manufacturer Property Server", "Generic"),
    SIG_MD_G_USER_PROP_S((short) 0x1013, "Generic User Property Server", "Generic"),
    SIG_MD_G_CLIENT_PROP_S((short) 0x1014, "Generic Client Property Server", "Generic"),
    SIG_MD_G_PROP_C((short) 0x1015, "Generic Property Client", "Generic"), // original: SIG_MD_G_PROP_S (may be err)

    SIG_MD_SENSOR_S((short) 0x1100, "Sensor Server", "Sensors"),
    SIG_MD_SENSOR_SETUP_S((short) 0x1101, "Sensor Setup Server", "Sensors"),
    SIG_MD_SENSOR_C((short) 0x1102, "Sensor Client", "Sensors"),

    SIG_MD_TIME_S((short) 0x1200, "Time Server", "Time and Scenes"),
    SIG_MD_TIME_SETUP_S((short) 0x1201, "Time Setup Server", "Time and Scenes"),
    SIG_MD_TIME_C((short) 0x1202, "Time Client", "Time and Scenes"),
    SIG_MD_SCENE_S((short) 0x1203, "Scene Server", "Time and Scenes"),
    SIG_MD_SCENE_SETUP_S((short) 0x1204, "Scene Setup Server", "Time and Scenes"),
    SIG_MD_SCENE_C((short) 0x1205, "Scene Client", "Time and Scenes"),
    SIG_MD_SCHED_S((short) 0x1206, "MeshScheduler Server", "Time and Scenes"),
    SIG_MD_SCHED_SETUP_S((short) 0x1207, "MeshScheduler Setup Server", "Time and Scenes"),
    SIG_MD_SCHED_C((short) 0x1208, "MeshScheduler Client", "Time and Scenes"),

    SIG_MD_LIGHTNESS_S((short) 0x1300, "Light Lightness Server", "Lighting"),
    SIG_MD_LIGHTNESS_SETUP_S((short) 0x1301, "Light Lightness Setup Server", "Lighting"),
    SIG_MD_LIGHTNESS_C((short) 0x1302, "Light Lightness Client", "Lighting"),
    SIG_MD_LIGHT_CTL_S((short) 0x1303, "Light CTL Server", "Lighting"),
    SIG_MD_LIGHT_CTL_SETUP_S((short) 0x1304, "Light CTL Setup Server", "Lighting"),
    SIG_MD_LIGHT_CTL_C((short) 0x1305, "Light CTL Client", "Lighting"),
    SIG_MD_LIGHT_CTL_TEMP_S((short) 0x1306, "Light CTL Temperature Server", "Lighting"),
    SIG_MD_LIGHT_HSL_S((short) 0x1307, "Light HSL Server", "Lighting"),
    SIG_MD_LIGHT_HSL_SETUP_S((short) 0x1308, "Light HSL Setup Server", "Lighting"),
    SIG_MD_LIGHT_HSL_C((short) 0x1309, "Light HSL Client", "Lighting"),
    SIG_MD_LIGHT_HSL_HUE_S((short) 0x130A, "Light HSL Hue Server", "Lighting"),
    SIG_MD_LIGHT_HSL_SAT_S((short) 0x130B, "Light HSL Saturation Server", "Lighting"),
    SIG_MD_LIGHT_XYL_S((short) 0x130C, "Light xyL Server", "Lighting"),
    SIG_MD_LIGHT_XYL_SETUP_S((short) 0x130D, "Light xyL Setup Server", "Lighting"),
    SIG_MD_LIGHT_XYL_C((short) 0x130E, "Light xyL Client", "Lighting"),
    SIG_MD_LIGHT_LC_S((short) 0x130F, "Light LC Server", "Lighting"),
    SIG_MD_LIGHT_LC_SETUP_S((short) 0x1310, "Light LC Setup Server", "Lighting"),
    SIG_MD_LIGHT_LC_C((short) 0x1311, "Light LC Client", "Lighting"),;

    /**
     * sig model id
     * 2 bytes
     */
    public short modelId;

    /**
     * model name
     */
    public String modelName;

    /**
     * model group desc
     */
    public String group;
    public boolean defaultSelected = false;

    public boolean selected;

    SigMeshModel(short modelId, String modelName, String group) {
        this(modelId, modelName, group, true);
    }

    SigMeshModel(short modelId, String modelName, String group, boolean enable) {
        this.modelId = modelId;
        this.modelName = modelName;
        this.group = group;
        this.defaultSelected = enable;
    }

    public static SigMeshModel[] getDefaultSelectList() {
        /*return new SigMeshModel[]{SIG_MD_G_ONOFF_S, SIG_MD_G_LEVEL_S, SIG_MD_G_DEF_TRANSIT_TIME_S,
                SIG_MD_LIGHTNESS_S, SIG_MD_LIGHTNESS_SETUP_S, SIG_MD_LIGHT_CTL_S,
                SIG_MD_LIGHT_CTL_SETUP_S, SIG_MD_LIGHT_CTL_TEMP_S,
                SIG_MD_LIGHT_LC_S, SIG_MD_LIGHT_LC_SETUP_S};*/

        List<SigMeshModel> sigMeshModels = new ArrayList<>();

        for (SigMeshModel model : values()) {
            if (model.defaultSelected) {
                sigMeshModels.add(model);
            }
        }

        return sigMeshModels.toArray(new SigMeshModel[]{});
//        return new SigMeshModel[]{SIG_MD_HEALTH_SERVER};
//        return values();

    }

    // default sub list
    public static SigMeshModel[] getDefaultSubList() {
        return new SigMeshModel[]{SIG_MD_G_ONOFF_S, SIG_MD_LIGHTNESS_S, SIG_MD_LIGHT_CTL_S,
                SIG_MD_LIGHT_CTL_TEMP_S, SIG_MD_LIGHT_HSL_S};

    }

    public static SigMeshModel getById(int id) {
        for (SigMeshModel model :
                values()) {
            if (model.modelId == id) return model;
        }
        return null;
    }


}

