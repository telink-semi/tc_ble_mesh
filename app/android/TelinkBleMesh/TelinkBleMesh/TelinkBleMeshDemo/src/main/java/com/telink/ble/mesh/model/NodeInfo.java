/********************************************************************************************************
 * @file NodeInfo.java
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

import android.os.Handler;
import android.util.SparseBooleanArray;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Element;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.db.Scheduler;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

import io.objectbox.annotation.Convert;
import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;
import io.objectbox.annotation.Transient;
import io.objectbox.relation.ToMany;
import io.objectbox.relation.ToOne;

/**
 * Created by kee on 2019/8/22.
 */

@Entity
public class NodeInfo implements Serializable {
    public static final String LOCAL_DEVICE_KEY = "00112233445566778899AABBCCDDEEFF";
    @Id
    public long id;

    public String name;

    /**
     * primary element unicast address
     */
    public int meshAddress;

    /**
     * mac address
     */
    public String macAddress;

    /**
     * device-uuid from scan-record when normal provision
     * or
     * device scan report when fast-provision or remote-provision
     */
    public byte[] deviceUUID;

    /**
     * network key indexes, contains at least one primary network key.
     * can be add by {@link com.telink.ble.mesh.core.message.config.NetKeyAddMessage}
     * big endian hex string
     */
    public List<String> netKeyIndexes = new ArrayList<>();

    /**
     * element count
     */
    public int elementCnt = 0;

    public boolean bound = false;

    public byte[] deviceKey;

    /**
     * Whether periodic broadcast is configured
     */
    public boolean timePublishConfigured;

    /**
     * device subscription/group info
     * hex string list, bigEndian
     */
    public List<String> subList = new ArrayList<>();

    // device lightness
    public int lum = 100;

    // device temperature
    public int temp = 0;

    /**
     * device on off state
     * 0:off 1:on -1:offline
     */
    @Convert(converter = OnlineStateConverter.class, dbType = Integer.class)
    private OnlineState onlineState = OnlineState.OFFLINE;

    /**
     * composition dataNodeInfo
     * {@link com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage}
     */
//    @Transient
    @Convert(converter = CompositionDataConverter.class, dbType = byte[].class)
    public CompositionData compositionData = null;

//    private byte[] cpsDataRaw;


    /**
     * scheduler
     */
    public ToMany<Scheduler> schedulers;

    /**
     * publication
     */
    private ToOne<PublishModel> publishModel;

    /**
     * default bind support
     */
    private boolean defaultBind = false;

    /**
     * subnet bridge enable
     */
    public boolean subnetBridgeEnabled = false;


    public ToMany<BridgingTable> bridgingTableList;

    /**
     * for UI selection
     */
    public boolean selected = false;


    /**
     * configs
     */

    // default TTL
    public byte defaultTTL = 0x0A;

    // is relay enabled
    public boolean relayEnable = true;

    // relay retransmit, include count and steps
    public byte relayRetransmit = 0x15;

    // is secure network beacon opened
    public boolean beaconOpened = true;

    // is gatt proxy enabled
    public boolean gattProxyEnable = true;

    // is friend enabled
    public boolean friendEnable = true;

    // network retransmit
    public byte networkRetransmit = 0x15;


    /**
     * direct forwarding enabled
     */
    public boolean directForwardingEnabled = false;

    public boolean directRelay = false;

    public boolean directProxyEnabled = false;

    public boolean directFriend = false;

    public boolean privateBeaconOpened = false;

    public boolean privateGattProxyEnable = false;

    @Transient
    private OfflineCheckTask offlineCheckTask = (OfflineCheckTask) () -> {
        onlineState = OnlineState.OFFLINE;
        MeshLogger.log("offline check task running");
        TelinkMeshApplication.getInstance().dispatchEvent(new NodeStatusChangedEvent(TelinkMeshApplication.getInstance(), NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, NodeInfo.this));
    };

    public OnlineState getOnlineState() {
        return onlineState;
    }

    public void setOnlineState(OnlineState onlineState) {
        this.onlineState = onlineState;
        PublishModel pm = publishModel.getTarget();
        if (pm != null) {
            Handler handler = TelinkMeshApplication.getInstance().getOfflineCheckHandler();
            handler.removeCallbacks(offlineCheckTask);
            int timeout = pm.period * 3 + 2000;
            if (this.onlineState != OnlineState.OFFLINE && timeout > 0) {
                handler.postDelayed(offlineCheckTask, timeout);
            }
        }
    }

    public boolean isPubSet() {
        return publishModel.getTarget() != null;
    }

    /**
     * used in db
     */
    public ToOne<PublishModel> getPublishModel() {
        return publishModel;
    }

    public PublishModel getPublishModelTarget() {
        return publishModel.getTarget();
    }

    public void setPublishModel(PublishModel model) {
        this.publishModel.setTarget(model);

        Handler handler = TelinkMeshApplication.getInstance().getOfflineCheckHandler();
        handler.removeCallbacks(offlineCheckTask);
        if (this.publishModel.getTarget() != null && this.onlineState != OnlineState.OFFLINE) {
            int timeout = publishModel.getTarget().period * 3 + 2000;
            if (timeout > 0) {
                handler.postDelayed(offlineCheckTask, timeout);
            }
        }
    }

    public Scheduler getSchedulerByIndex(byte index) {
        if (schedulers == null || schedulers.size() == 0) {
            return null;
        }
        for (Scheduler scheduler : schedulers) {
            if (scheduler.getIndex() == index) {
                return scheduler;
            }
        }
        return null;
    }

    public void saveScheduler(Scheduler scheduler) {
        for (int i = 0; i < schedulers.size(); i++) {
            if (schedulers.get(i).getIndex() == scheduler.getIndex()) {
                schedulers.set(i, scheduler);
                return;
            }
        }
        schedulers.add(scheduler);
    }

    // 0 - 15/0x0f
    public byte allocSchedulerIndex() {
        if (schedulers == null || schedulers.size() == 0) {
            return 0;
        }

        // find the value that not used
        outer:
        for (byte i = 0; i <= 0x0f; i++) {
            for (Scheduler scheduler : schedulers) {
                if (scheduler.getIndex() == i) {
                    continue outer;
                }
            }
            return i;
        }
        return -1;
    }

    /**
     * get on/off model element info
     * in panel , multi on/off may exist in different element
     *
     * @return element adr list
     */
    public List<Integer> getEleListByModel(int targetModelId) {
        if (compositionData == null) return null;
        List<Integer> addressList = new ArrayList<>();

        // element address is based on primary address and increase in loop
        int eleAdr = this.meshAddress;
        outer:
        for (Element element : compositionData.elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == targetModelId) {
                        addressList.add(eleAdr++);
                        continue outer;
                    }
                }
            }
            eleAdr++;
        }

        return addressList;
    }

    /**
     * @param tarModelId target model id
     * @return element address: -1 err
     */
    public int getTargetEleAdr(int tarModelId) {
        if (compositionData == null) return -1;
        int eleAdr = this.meshAddress;
        for (Element element : compositionData.elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            if (element.vendorModels != null) {
                for (int modelId : element.vendorModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            eleAdr++;
        }
        return -1;
    }


    /**
     * @param associatedModelId target model id {@link MeshSigModel#getLevelAssociatedList()}
     * @return element address: -1 err
     */
    public int getLevelAssociatedEleAdr(int associatedModelId) {
        if (compositionData == null) return -1;
        int eleAdr = this.meshAddress;
        for (Element element : compositionData.elements) {
            if (element.sigModels != null) {
                if (element.sigModels.contains(associatedModelId) && element.sigModels.contains(MeshSigModel.SIG_MD_G_LEVEL_S.modelId)) {
                    return eleAdr;
                }
            }
            eleAdr++;
        }
        return -1;
    }

    /**
     * get lum model element
     *
     * @return lum lightness union info
     */
    public SparseBooleanArray getLumEleInfo() {
        if (compositionData == null) return null;
        int eleAdr = this.meshAddress;

        SparseBooleanArray result = new SparseBooleanArray();

        for (Element element : compositionData.elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean lumSupport = false;
                // if contains lightness model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHTNESS_S.modelId) {
                        lumSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (lumSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }

    /**
     * get element with temperature model
     *
     * @return temp & isLevelSupported
     */
    public SparseBooleanArray getTempEleInfo() {
        if (compositionData == null) return null;
        int eleAdr = this.meshAddress;

        SparseBooleanArray result = new SparseBooleanArray();

        for (Element element : compositionData.elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean tempSupport = false;
                // contains temperature model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId) {
                        tempSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (tempSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }


    public String getPidDesc() {
        String pidInfo = "";
        if (bound && compositionData != null) {
            return "cid-" +
                    Arrays.bytesToHexString(MeshUtils.integer2Bytes(compositionData.cid, 2, ByteOrder.LITTLE_ENDIAN), "") +
                    " pid-" +
                    Arrays.bytesToHexString(MeshUtils.integer2Bytes(compositionData.pid, 2, ByteOrder.LITTLE_ENDIAN), "");
//            pidInfo = (compositionData.cid == 0x0211 ? String.format("%04X", compositionData.pid)
//                    : "cid-" + String.format("%02X", compositionData.cid));

        } else {
            pidInfo = "(unbound)";
        }
        return pidInfo;
    }

    public boolean isDefaultBind() {
        return defaultBind;
    }

    public void setDefaultBind(boolean defaultBind) {
        this.defaultBind = defaultBind;
    }

    public boolean isLpn() {
        return this.compositionData != null && this.compositionData.lowPowerSupport();
    }

    /**
     * is node offline
     */
    public boolean isOffline() {
        return this.onlineState == OnlineState.OFFLINE;
    }


    /**
     * is node on
     */
    public boolean isOn() {
        return this.onlineState == OnlineState.ON;
    }


    /**
     * is node off
     */
    public boolean isOff() {
        return this.onlineState == OnlineState.OFF;
    }

    public void save() {
        MeshInfoService.getInstance().updateNodeInfo(this);
    }
}
