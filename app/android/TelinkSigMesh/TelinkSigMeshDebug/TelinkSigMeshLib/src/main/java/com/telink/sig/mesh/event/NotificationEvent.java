/********************************************************************************************************
 * @file NotificationEvent.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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

import com.telink.sig.mesh.light.Opcode;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.util.Arrays;


/**
 * Created by kee on 2017/8/30.
 */

public class NotificationEvent extends Event<String> {

    /****************************
     * default notifications
     *****************************/
    /**
     * device on off status
     */
    public static final String EVENT_TYPE_DEVICE_ON_OFF_STATUS = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_ON_OFF_STATUS";

    /**
     * level status
     */
    public static final String EVENT_TYPE_DEVICE_LEVEL_STATUS = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_LEVEL_STATUS";

    /**
     * group info
     */
    public static final String EVENT_TYPE_GROUP_INFO = "com.telink.sig.mesh.EVENT_TYPE_GROUP_INFO";

    /**
     * lightness status
     */
    public static final String EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY = "com.telink.sig.mesh.EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY";

    /**
     * temperature status
     */
    public static final String EVENT_TYPE_TEMP_STATUS_NOTIFY = "com.telink.sig.mesh.EVENT_TYPE_TEMP_STATUS_NOTIFY";

    public static final String EVENT_TYPE_CTL_STATUS_NOTIFY = "com.telink.sig.mesh.EVENT_TYPE_CTL_STATUS_NOTIFY";

    public static final String EVENT_TYPE_SUB_OP_CONFIRM = "com.telink.sig.mesh.EVENT_TYPE_SUB_OP_CONFIRM";

    /**
     * reset success
     */
    public static final String EVENT_TYPE_KICK_OUT_CONFIRM = "com.telink.sig.mesh.EVENT_TYPE_KICK_OUT_CONFIRM";

    public static final String EVENT_TYPE_SCHEDULER_STATUS = "com.telink.sig.mesh.EVENT_TYPE_SCHEDULER_STATUS";

    public static final String EVENT_TYPE_SCENE_REGISTER_STATUS = "com.telink.sig.mesh.EVENT_TYPE_SCENE_REGISTER_STATUS";

    public static final String EVENT_TYPE_PUBLICATION_STATUS = "com.telink.sig.mesh.EVENT_TYPE_PUBLICATION_STATUS";

    public static final String EVENT_TYPE_RELAY_STATUS = "com.telink.sig.mesh.EVENT_TYPE_RELAY_STATUS";

    public static final String EVENT_TYPE_REMOTE_SCAN_REPORT = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_SCAN_REPORT";

    public static final String EVENT_TYPE_TIME_STATUS = "com.telink.sig.mesh.EVENT_TYPE_TIME_STATUS";

    /**
     * mesh ota status
     */
    public static final String EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS = "com.telink.sig.mesh.EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS";

    public static final String EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS = "com.telink.sig.mesh.EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS";

    public static final String EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS = "com.telink.sig.mesh.EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS";

    /**
     * vendor response
     * @see com.telink.sig.mesh.model.OpcodeType#VENDOR
     */
    public static final String EVENT_TYPE_VENDOR_RESPONSE = "com.telink.sig.mesh.EVENT_TYPE_VENDOR_RESPONSE";

    /**
     * invalid data
     */
    public static final String EVENT_TYPE_DATA_ERR = "com.telink.sig.mesh.EVENT_TYPE_DATA_ERR";

    /**
     * unknown status
     */
    public static final String EVENT_TYPE_UNKNOWN = "com.telink.sig.mesh.EVENT_TYPE_UNKNOWN";


    private byte[] rawData;

    private NotificationInfo notificationInfo;

    private boolean statusChanged = false;

    public NotificationEvent(Object sender, byte[] data, NotificationInfo notificationInfo) {
        super(sender, null);
        this.rawData = data;
        this.notificationInfo = notificationInfo;
        type = getNotificationType();
    }

    public NotificationEvent(Object sender, String type) {
        super(sender, type);
    }

    public boolean isStatusChanged() {
        return statusChanged;
    }

    public void setStatusChanged(boolean statusChanged) {
        this.statusChanged = statusChanged;
    }

    private String getNotificationType() {
        if (this.notificationInfo == null) {
            return EVENT_TYPE_DATA_ERR;
        }
        int opcode = notificationInfo.opcode;

        if (notificationInfo.isVendor) {
            return EVENT_TYPE_VENDOR_RESPONSE;
        }
        String eventType = EVENT_TYPE_UNKNOWN;
        if (opcode == Opcode.G_LEVEL_STATUS.getValue()) {
            // 设备亮度信息
            eventType = EVENT_TYPE_DEVICE_LEVEL_STATUS;
        } else if (opcode == Opcode.G_ONOFF_STATUS.getValue()) {
            // 设备开关等信息
            eventType = EVENT_TYPE_DEVICE_ON_OFF_STATUS;
        } else if (opcode == Opcode.CFG_SIG_MODEL_SUB_LIST.getValue()) {
            // 分组信息
            eventType = EVENT_TYPE_GROUP_INFO;
        } else if (opcode == Opcode.LIGHTNESS_STATUS.getValue()) {
            eventType = EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY;
        } else if (opcode == Opcode.LIGHT_CTL_TEMP_STATUS.getValue()) {
            eventType = EVENT_TYPE_TEMP_STATUS_NOTIFY;
        } else if (opcode == Opcode.LIGHT_CTL_STATUS.getValue()) {
            eventType = EVENT_TYPE_CTL_STATUS_NOTIFY;
        } else if (opcode == Opcode.CFG_MODEL_SUB_STATUS.getValue()) {
            eventType = EVENT_TYPE_SUB_OP_CONFIRM;
        } else if (opcode == Opcode.NODE_RESET_STATUS.getValue()) {
            eventType = EVENT_TYPE_KICK_OUT_CONFIRM;
        } else if (opcode == Opcode.SCHD_ACTION_STATUS.getValue()) {
            eventType = EVENT_TYPE_SCHEDULER_STATUS;
        } else if (opcode == Opcode.SCENE_REG_STATUS.getValue()) {
            eventType = EVENT_TYPE_SCENE_REGISTER_STATUS;
        } else if (opcode == Opcode.CFG_MODEL_PUB_STATUS.getValue()) {
            eventType = EVENT_TYPE_PUBLICATION_STATUS;
        } else if (opcode == Opcode.CFG_RELAY_STATUS.getValue()) {
            eventType = EVENT_TYPE_RELAY_STATUS;
        } else if (opcode == Opcode.TIME_STATUS.getValue()) {
            eventType = EVENT_TYPE_TIME_STATUS;
        } else if (opcode == Opcode.REMOTE_PROV_SCAN_REPORT.getValue()) {
            eventType = EVENT_TYPE_REMOTE_SCAN_REPORT;
        } else if (opcode == Opcode.FW_UPDATE_STATUS.getValue()) {
            eventType = EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS;
        } else if (opcode == Opcode.FW_DISTRIBUT_STATUS.getValue()) {
            eventType = EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS;
        } else if (opcode == Opcode.FW_INFO_STATUS.getValue()) {
            eventType = EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS;
        }
        return eventType;
    }

    public byte[] getRawData() {
        return rawData;
    }

    public NotificationInfo getNotificationInfo() {
        return notificationInfo;
    }


}
