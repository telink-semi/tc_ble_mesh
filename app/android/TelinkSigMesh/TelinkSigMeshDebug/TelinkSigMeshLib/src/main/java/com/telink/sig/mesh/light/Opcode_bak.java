/********************************************************************************************************
 * @file     Opcode.java 
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
package com.telink.sig.mesh.light;

import android.support.annotation.Nullable;

public enum Opcode_bak {

    OP_ONLINE_STATUS_NOTIFY_LEVEL(0x0882, "device online status lum"),
    OP_ONLINE_STATUS_NOTIFY_ON_OFF(0x0482, "device online status on off"),


    OP_LIGHTNESS_STATUS_NOTIFY(0x4E82, "device lum"),
    OP_CTL_TEMP_STATUS_NOTIFY(0x6682, "device temp"),
    OP_CTL_STATUS_NOTIFY(0x6082, "ctl status"),
    OP_GROUP_INFO_NOTIFY(0x2A80, "group info notify"),

    OP_CFG_MODEL_SUB_ADD(0x1B80, "add to sub list"),

    OP_CFG_MODEL_SUB_DEL(0x1C80, "delete from sub list"),

    OP_CFG_MODEL_SUB_CONFIRM(0x1F80, "confirm sub operation"),

    OP_NODE_RESET_STATUS(0x4A80, "Config Node Reset Status"),

    OP_SCHEDULER_STATUS(0x5F, "Scheduler Status"),

    OP_SCENE_REGISTER_STATUS(0x4582, "Scene Register Status"),

    OP_PUBLICATION_STATUS(0x1980, "Model Publication Status"),
    OP_RELAY_STATUS(0x2880, "Model Relay Status"),

    G_ON_OFF_GET(0x0182, "On Off Get"),

    G_ON_OFF_SET(0x0282, "On Off Set"),

    G_ON_OFF_SET_NO_ACK(0x0382, "On Off Set No ack"),

    ;

    private final int value;
    private final String info;

    Opcode_bak(int value, @Nullable String info) {
        this.value = value;
        this.info = info;
    }

    public int getValue() {
        return value;
    }

    public String getInfo() {
        return info;
    }
}
