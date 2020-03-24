package com.telink.ble.mesh.core.message;

import android.util.SparseArray;

import com.telink.ble.mesh.core.message.config.AppKeyStatusMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelAppStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.core.message.fastpv.MeshAddressStatusMessage;
import com.telink.ble.mesh.core.message.generic.LevelStatusMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.core.message.lighting.CtlStatusMessage;
import com.telink.ble.mesh.core.message.lighting.CtlTemperatureStatusMessage;
import com.telink.ble.mesh.core.message.lighting.HslStatusMessage;
import com.telink.ble.mesh.core.message.lighting.HslTargetStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessStatusMessage;
import com.telink.ble.mesh.core.message.rp.LinkStatusMessage;
import com.telink.ble.mesh.core.message.rp.ProvisioningPDUOutboundReportMessage;
import com.telink.ble.mesh.core.message.rp.ProvisioningPDUReportMessage;
import com.telink.ble.mesh.core.message.rp.ScanReportStatusMessage;
import com.telink.ble.mesh.core.message.rp.ScanStatusMessage;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.core.message.scene.SceneStatusMessage;
import com.telink.ble.mesh.core.message.scheduler.SchedulerActionStatusMessage;
import com.telink.ble.mesh.core.message.scheduler.SchedulerStatusMessage;
import com.telink.ble.mesh.core.message.time.TimeStatusMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareDistributionStatusMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareInfoStatusMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockTransferStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectInfoStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectTransferStatusMessage;

/**
 * changed from enum to class for outer importing
 * All registered StatusMessage should have empty constructor for [Creating Instance]
 * {@link StatusMessage#createByAccessMessage(int, byte[])}
 * Created by kee on 2019/9/3.
 */

public class MeshStatus {
    /**
     * status message opcode {@link com.telink.ble.mesh.core.networking.AccessLayerPDU#opcode}
     */
    private int opcode;

    /**
     * message info: Class extends StatusMessage
     *
     * @see StatusMessage
     */
    private Class statusMessageCls;

    public MeshStatus(int opcode, Class statusMessageCls) {
        this.opcode = opcode;
        this.statusMessageCls = statusMessageCls;
    }

    public Class getStatusMessageCls() {
        return statusMessageCls;
    }

    public void setStatusMessageCls(Class statusMessageCls) {
        this.statusMessageCls = statusMessageCls;
    }

    public int getOpcode() {
        return opcode;
    }

    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    public static class Container {

        private static SparseArray<Class> statusMap = new SparseArray<>();

        static {
            register(new MeshStatus(Opcode.COMPOSITION_DATA_STATUS.value, CompositionDataStatusMessage.class));
            register(new MeshStatus(Opcode.MODE_APP_STATUS.value, ModelAppStatusMessage.class));
            register(new MeshStatus(Opcode.APPKEY_STATUS.value, AppKeyStatusMessage.class));
            register(new MeshStatus(Opcode.NODE_RESET_STATUS.value, NodeResetStatusMessage.class));
            register(new MeshStatus(Opcode.CFG_MODEL_SUB_STATUS.value, ModelSubscriptionStatusMessage.class));
            register(new MeshStatus(Opcode.CFG_MODEL_PUB_STATUS.value, ModelPublicationStatusMessage.class));

            // generic
            register(new MeshStatus(Opcode.G_ONOFF_STATUS.value, OnOffStatusMessage.class));
            register(new MeshStatus(Opcode.G_LEVEL_STATUS.value, LevelStatusMessage.class));

            // lighting
            register(new MeshStatus(Opcode.LIGHTNESS_STATUS.value, LightnessStatusMessage.class));
            register(new MeshStatus(Opcode.LIGHT_CTL_TEMP_STATUS.value, CtlTemperatureStatusMessage.class));
            register(new MeshStatus(Opcode.LIGHT_CTL_STATUS.value, CtlStatusMessage.class));

            register(new MeshStatus(Opcode.LIGHT_HSL_STATUS.value, HslStatusMessage.class));
            register(new MeshStatus(Opcode.LIGHT_HSL_TARGET_STATUS.value, HslTargetStatusMessage.class));

            // time
            register(new MeshStatus(Opcode.TIME_STATUS.value, TimeStatusMessage.class));

            // scene
            register(new MeshStatus(Opcode.SCENE_STATUS.value, SceneStatusMessage.class));
            register(new MeshStatus(Opcode.SCENE_REG_STATUS.value, SceneRegisterStatusMessage.class));

            // scheduler status
            register(new MeshStatus(Opcode.SCHD_STATUS.value, SchedulerStatusMessage.class));
            register(new MeshStatus(Opcode.SCHD_ACTION_STATUS.value, SchedulerActionStatusMessage.class));


            // mesh firmware update
            register(new MeshStatus(Opcode.FW_INFO_STATUS.value, FirmwareInfoStatusMessage.class));
            register(new MeshStatus(Opcode.FW_DISTRIBUT_STATUS.value, FirmwareDistributionStatusMessage.class));
            register(new MeshStatus(Opcode.FW_UPDATE_STATUS.value, FirmwareUpdateStatusMessage.class));
            register(new MeshStatus(Opcode.OBJ_BLOCK_STATUS.value, ObjectBlockStatusMessage.class));
            register(new MeshStatus(Opcode.OBJ_BLOCK_TRANSFER_STATUS.value, ObjectBlockTransferStatusMessage.class));
            register(new MeshStatus(Opcode.OBJ_INFO_STATUS.value, ObjectInfoStatusMessage.class));
            register(new MeshStatus(Opcode.OBJ_TRANSFER_STATUS.value, ObjectTransferStatusMessage.class));

            // remote provision
            register(new MeshStatus(Opcode.REMOTE_PROV_SCAN_STS.value, ScanStatusMessage.class));
            register(new MeshStatus(Opcode.REMOTE_PROV_SCAN_REPORT.value, ScanReportStatusMessage.class));
            register(new MeshStatus(Opcode.REMOTE_PROV_LINK_STS.value, LinkStatusMessage.class));
            register(new MeshStatus(Opcode.REMOTE_PROV_PDU_REPORT.value, ProvisioningPDUReportMessage.class));
            register(new MeshStatus(Opcode.REMOTE_PROV_PDU_OUTBOUND_REPORT.value, ProvisioningPDUOutboundReportMessage.class));
//            register(new MeshStatus(Opcode.REMOTE_PROV_SCAN_CAPA_STS.value, ObjectTransferStatusMessage.class));

            // fast provision [vendor]
            register(new MeshStatus(Opcode.VD_MESH_ADDR_GET_STS.value, MeshAddressStatusMessage.class));
        }

        public static void register(MeshStatus status) {
            statusMap.put(status.opcode, status.statusMessageCls);
        }

        public static Class getMessageClass(int opcode) {
            return statusMap.get(opcode);
        }


    }
}
