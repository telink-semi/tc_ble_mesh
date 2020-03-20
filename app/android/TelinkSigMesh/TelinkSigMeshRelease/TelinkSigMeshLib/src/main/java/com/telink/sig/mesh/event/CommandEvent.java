/********************************************************************************************************
 * @file     CommandEvent.java 
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
package com.telink.sig.mesh.event;

import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.MeshCommand;

/**
 * Created by kee on 2017/8/30.
 */

public class CommandEvent extends Event<String> {

    /**
     * CMD busy, CMD send fail
     */
    public static final String EVENT_TYPE_CMD_ERROR_BUSY = "com.telink.sig.mesh.EVENT_TYPE_CMD_ERROR_BUSY";

    /**
     * CMD processing, waiting for callback
     */
    public static final String EVENT_TYPE_CMD_PROCESSING = "com.telink.sig.mesh.EVENT_TYPE_CMD_PROCESSING";

    /**
     * CMD callback success
     */
    public static final String EVENT_TYPE_CMD_COMPLETE = "com.telink.sig.mesh.EVENT_TYPE_CMD_COMPLETE";

    public static final String EVENT_TYPE_CMD_ERROR_UNKNOWN = "com.telink.sig.mesh.EVENT_TYPE_CMD_ERROR_UNKNOWN";

    private MeshCommand meshCommand;

    public CommandEvent(Object sender, String type, MeshCommand meshCommand) {
        super(sender, type);
        this.meshCommand = meshCommand;
    }

    public MeshCommand getMeshCommand() {
        return meshCommand;
    }

    public CommandEvent(Object sender, String type) {
        super(sender, type);
    }
}
