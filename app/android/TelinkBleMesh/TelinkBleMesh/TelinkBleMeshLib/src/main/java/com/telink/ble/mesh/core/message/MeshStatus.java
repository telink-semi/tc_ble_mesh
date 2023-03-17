/********************************************************************************************************
 * @file MeshStatus.java
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

import android.util.SparseArray;

import com.telink.ble.mesh.core.message.aggregator.OpcodeAggregatorStatusMessage;
import com.telink.ble.mesh.core.message.config.AppKeyStatusMessage;
import com.telink.ble.mesh.core.message.config.BeaconStatusMessage;
import com.telink.ble.mesh.core.message.config.BridgingTableStatusMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLStatusMessage;
import com.telink.ble.mesh.core.message.config.FriendStatusMessage;
import com.telink.ble.mesh.core.message.config.GattProxyStatusMessage;
import com.telink.ble.mesh.core.message.config.KeyRefreshPhaseStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelAppStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionStatusMessage;
import com.telink.ble.mesh.core.message.config.NetKeyStatusMessage;
import com.telink.ble.mesh.core.message.config.NetworkTransmitStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentityStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.core.message.config.RelayStatusMessage;
import com.telink.ble.mesh.core.message.config.SubnetBridgeStatusMessage;
import com.telink.ble.mesh.core.message.fastpv.MeshAddressStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCapabilitiesStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDFirmwareStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversListMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDUploadStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareMetadataStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobBlockStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobPartialBlockReportMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobTransferStatusMessage;
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

/**
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
            register(Opcode.COMPOSITION_DATA_STATUS.value, CompositionDataStatusMessage.class);
            register(Opcode.MODE_APP_STATUS.value, ModelAppStatusMessage.class);
            register(Opcode.APPKEY_STATUS.value, AppKeyStatusMessage.class);
            register(Opcode.NODE_RESET_STATUS.value, NodeResetStatusMessage.class);
            register(Opcode.CFG_MODEL_SUB_STATUS.value, ModelSubscriptionStatusMessage.class);
            register(Opcode.CFG_MODEL_PUB_STATUS.value, ModelPublicationStatusMessage.class);
            register(Opcode.NODE_ID_STATUS.value, NodeIdentityStatusMessage.class);
            register(Opcode.NETKEY_STATUS.value, NetKeyStatusMessage.class);

            register(Opcode.SUBNET_BRIDGE_STATUS.value, SubnetBridgeStatusMessage.class);
            register(Opcode.BRIDGING_TABLE_STATUS.value, BridgingTableStatusMessage.class);

            /// configs
            register(Opcode.CFG_NW_TRANSMIT_STATUS.value, NetworkTransmitStatusMessage.class);
            register(Opcode.CFG_BEACON_STATUS.value, BeaconStatusMessage.class);
            register(Opcode.CFG_DEFAULT_TTL_STATUS.value, DefaultTTLStatusMessage.class);
            register(Opcode.CFG_RELAY_STATUS.value, RelayStatusMessage.class);

            register(Opcode.CFG_FRIEND_STATUS.value, FriendStatusMessage.class);
            register(Opcode.CFG_GATT_PROXY_STATUS.value, GattProxyStatusMessage.class);
            register(Opcode.CFG_KEY_REFRESH_PHASE_STATUS.value, KeyRefreshPhaseStatusMessage.class);

            // generic
            register(Opcode.G_ONOFF_STATUS.value, OnOffStatusMessage.class);
            register(Opcode.G_LEVEL_STATUS.value, LevelStatusMessage.class);

            // lighting
            register(Opcode.LIGHTNESS_STATUS.value, LightnessStatusMessage.class);
            register(Opcode.LIGHT_CTL_TEMP_STATUS.value, CtlTemperatureStatusMessage.class);
            register(Opcode.LIGHT_CTL_STATUS.value, CtlStatusMessage.class);

            register(Opcode.LIGHT_HSL_STATUS.value, HslStatusMessage.class);
            register(Opcode.LIGHT_HSL_TARGET_STATUS.value, HslTargetStatusMessage.class);

            // time
            register(Opcode.TIME_STATUS.value, TimeStatusMessage.class);

            // scene
            register(Opcode.SCENE_STATUS.value, SceneStatusMessage.class);
            register(Opcode.SCENE_REG_STATUS.value, SceneRegisterStatusMessage.class);

            // scheduler status
            register(Opcode.SCHD_STATUS.value, SchedulerStatusMessage.class);
            register(Opcode.SCHD_ACTION_STATUS.value, SchedulerActionStatusMessage.class);


            // mesh firmware update
            register(Opcode.FIRMWARE_UPDATE_INFORMATION_STATUS.value, FirmwareUpdateInfoStatusMessage.class);
            register(Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_STATUS.value, FirmwareMetadataStatusMessage.class);
            register(Opcode.FIRMWARE_UPDATE_STATUS.value, FirmwareUpdateStatusMessage.class);

            // mesh firmware distribution
            register(Opcode.FD_STATUS.value, FDStatusMessage.class);
            register(Opcode.FD_CAPABILITIES_STATUS.value, FDCapabilitiesStatusMessage.class);
            register(Opcode.FD_RECEIVERS_STATUS.value, FDReceiversStatusMessage.class);
            register(Opcode.FD_RECEIVERS_LIST.value, FDReceiversListMessage.class);
            register(Opcode.FD_UPLOAD_STATUS.value, FDUploadStatusMessage.class);
            register(Opcode.FD_FIRMWARE_STATUS.value, FDFirmwareStatusMessage.class);

            /// blob transfer
            register(Opcode.BLOB_BLOCK_STATUS.value, BlobBlockStatusMessage.class);
            register(Opcode.BLOB_INFORMATION_STATUS.value, BlobInfoStatusMessage.class);
            register(Opcode.BLOB_TRANSFER_STATUS.value, BlobTransferStatusMessage.class);
            register(Opcode.BLOB_PARTIAL_BLOCK_REPORT.value, BlobPartialBlockReportMessage.class);
            /*register(Opcode.FW_INFO_STATUS.value, FirmwareInfoStatusMessage.class);
            register(Opcode.FW_DISTRIBUT_STATUS.value, FirmwareDistributionStatusMessage.class);
            register(Opcode.FW_UPDATE_STATUS.value, FirmwareUpdateStatusMessage.class);
            register(Opcode.OBJ_BLOCK_STATUS.value, ObjectBlockStatusMessage.class);
            register(Opcode.OBJ_BLOCK_TRANSFER_STATUS.value, ObjectBlockTransferStatusMessage.class);
            register(Opcode.OBJ_INFO_STATUS.value, ObjectInfoStatusMessage.class);
            register(Opcode.OBJ_TRANSFER_STATUS.value, ObjectTransferStatusMessage.class);*/

            // remote provision
            register(Opcode.REMOTE_PROV_SCAN_STS.value, ScanStatusMessage.class);
            register(Opcode.REMOTE_PROV_SCAN_REPORT.value, ScanReportStatusMessage.class);
            register(Opcode.REMOTE_PROV_LINK_STS.value, LinkStatusMessage.class);
            register(Opcode.REMOTE_PROV_PDU_REPORT.value, ProvisioningPDUReportMessage.class);
            register(Opcode.REMOTE_PROV_PDU_OUTBOUND_REPORT.value, ProvisioningPDUOutboundReportMessage.class);
//            register(Opcode.REMOTE_PROV_SCAN_CAPA_STS.value, ObjectTransferStatusMessage.class);

            // fast provision [vendor]
            register(Opcode.VD_MESH_ADDR_GET_STS.value, MeshAddressStatusMessage.class);

            // opcode aggregator
            register(Opcode.OP_AGGREGATOR_STATUS.value, OpcodeAggregatorStatusMessage.class);
        }

        public static void register(MeshStatus status) {
            statusMap.put(status.opcode, status.statusMessageCls);
        }

        public static void register(int opcode, Class statusMessageCls) {
            statusMap.put(opcode, statusMessageCls);
        }

        public static Class getMessageClass(int opcode) {
            return statusMap.get(opcode);
        }


    }
}
