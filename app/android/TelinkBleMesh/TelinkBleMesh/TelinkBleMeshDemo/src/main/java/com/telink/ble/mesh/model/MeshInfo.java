/********************************************************************************************************
 * @file MeshInfo.java
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


import android.content.Context;
import android.util.SparseArray;

import androidx.annotation.NonNull;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.NetworkLayerPDU;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.json.AddressRange;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.model.json.Provisioner;
import com.telink.ble.mesh.model.json.SceneRange;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.Locale;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;
import io.objectbox.relation.ToMany;

/**
 * Created by kee on 2019/8/22.
 */

@Entity
public class MeshInfo implements Serializable, Cloneable {

    public static final String MESH_NAME_DEFAULT = "Telink-SIG-mesh";

    public static final String PROVISIONER_NAME_DEFAULT = "Default-Provisioner";

    @Id
    public long id;

    /**
     * the same with {@link com.telink.ble.mesh.model.json.MeshStorage#meshName}
     */
    public String meshName;

    /**
     * the same with {@link com.telink.ble.mesh.model.json.MeshStorage#timestamp}
     */
    public String timestamp;

    /**
     * if the {@link #ivIndex} is uninitialized, provision is not permitted
     * once received the ivIndex in beacon , the ivIndex should be replaced
     */
    public static final int UNINITIALIZED_IVI = -1;

    /**
     * save meshUUID in json
     */
    public String meshUUID;

    /**
     * local provisioner UUID
     */
    public String provisionerUUID;

    /**
     * unicast address range
     */

    public ToMany<AddressRange> unicastRange;

    /**
     * nodes saved in mesh network
     */
    public ToMany<NodeInfo> nodes;

    /**
     * network key and network key index
     */
    public ToMany<MeshNetKey> meshNetKeyList;

    /**
     * application key list
     */
    public ToMany<MeshAppKey> appKeyList;

    /**
     * ivIndex and sequence number are used in NetworkPDU
     *
     * @see NetworkLayerPDU#getSeq()
     * <p>
     * should be updated and saved when {@link NetworkInfoUpdateEvent} received
     */
    public int ivIndex = UNINITIALIZED_IVI;

    /**
     * provisioner sequence number
     */
    public int sequenceNumber;

    /**
     * provisioner address
     */
    public int localAddress;

    /**
     * unicast address prepared for node provisioning
     * increase by [element count] when provisioning success
     *
     * @see NodeInfo#elementCnt
     */
    private int provisionIndex = 1;

    public int addressTopLimit = 0xFF;

    /**
     * scenes saved in mesh
     */
    public ToMany<Scene> scenes;

    /**
     * groups (group address 0xC000~0xC0FF)
     */
    public ToMany<GroupInfo> groups;

    /**
     * extend groups
     */
    public ToMany<GroupInfo> extendGroups;

    /**
     * all provisioners , include local provisioner and others parsed from json
     */
    public ToMany<Provisioner> allProvisioners;

    /**
     * excluded nodes, not show on UI
     */
    public ToMany<NodeInfo> excludedNodes;


    public ToMany<NodeInfo> provisionerNodes;

    // solicitation sequence number
    public int solSeq = 1;
    /**
     * static-oob info
     */
//    public ToMany<OobInfo> oobInfos;
    public MeshNetKey getDefaultNetKey() {
        return meshNetKeyList.get(0);
    }

    public int getDefaultAppKeyIndex() {
        if (appKeyList.size() == 0) {
            return 0;
        }
        return appKeyList.get(0).index;
    }

    public NodeInfo getDeviceByMeshAddress(int meshAddress) {
        if (this.nodes == null)
            return null;

        for (NodeInfo info : nodes) {
            if (info.meshAddress == meshAddress)
                return info;
        }
        return null;
    }

    /**
     * @param deviceUUID 16 bytes uuid
     */
    public NodeInfo getDeviceByUUID(@NonNull byte[] deviceUUID) {
        for (NodeInfo info : nodes) {
            if (Arrays.equals(deviceUUID, info.deviceUUID))
                return info;
        }
        return null;
    }

    public void insertDevice(NodeInfo deviceInfo, boolean updatePvIndex) {
        NodeInfo local = getDeviceByUUID(deviceInfo.deviceUUID);
        if (local != null) {
            this.removeDeviceByUUID(deviceInfo.deviceUUID);
        }
        nodes.add(deviceInfo);
        if (updatePvIndex) {
            increaseProvisionIndex(deviceInfo.elementCnt);
        } else {
            saveOrUpdate();
        }
    }


    public void removeNode(NodeInfo node) {
        if (this.nodes.size() == 0) return;
        for (Scene scene : scenes) {
            if (scene.remove(node)) {
                MeshInfoService.getInstance().updateScene(scene);
            }
        }
        this.nodes.remove(node);
        saveOrUpdate();
    }

    public boolean removeDeviceByUUID(byte[] deviceUUID) {

        if (this.nodes == null || this.nodes.size() == 0) return false;
        Iterator<NodeInfo> iterator = nodes.iterator();
        while (iterator.hasNext()) {
            NodeInfo deviceInfo = iterator.next();
            if (Arrays.equals(deviceUUID, deviceInfo.deviceUUID)) {
                iterator.remove();
                return true;
            }
        }

        return false;
    }


    /**
     * get all online nodes
     */
    public int getOnlineCountInAll() {
        if (nodes == null || nodes.size() == 0) {
            return 0;
        }

        int result = 0;
        for (NodeInfo device : nodes) {
            if (!device.isOffline()) {
                result++;
            }
        }

        return result;
    }

    /**
     * get online nodes count in group
     *
     * @return
     */
    public int getOnlineCountInGroup(int groupAddress) {
        if (nodes == null || nodes.size() == 0) {
            return 0;
        }
        int result = 0;
        for (NodeInfo device : nodes) {
            if (!device.isOffline()) {
                for (String addr : device.subList) {
                    if (MeshUtils.hexToIntB(addr) == groupAddress) {
                        result++;
                        break;
                    }
                }
            }
        }

        return result;
    }

    public Scene getSceneById(int id) {
        for (Scene scene : scenes) {
            if (id == scene.sceneId) {
                return scene;
            }
        }
        return null;
    }

    /**
     * 1-0xFFFF
     *
     * @return -1 invalid id
     */
    public int allocSceneId() {
        if (scenes.size() == 0) {
            return 1;
        }
        int id = scenes.get(scenes.size() - 1).sceneId;
        if (id == 0xFFFF) {
            return -1;
        }
        return id + 1;
    }

    public void addScene(Scene scene) {
        this.scenes.add(scene);
        this.saveOrUpdate();
    }

    public void removeScene(Scene scene) {
        this.scenes.remove(scene);
        this.saveOrUpdate();
    }

    // only update metadata in mesh info, not includes inner entities (ToOne and ToMany)
    public void saveOrUpdate() {
        MeshInfoService.getInstance().updateMeshInfo(this);
    }


    @Override
    public String toString() {
        return "MeshInfo{" +
                "id=" + id +
                "nodes=" + nodes.size() +
                ", netKey=" + getNetKeyStr() +
                ", appKey=" + getAppKeyStr() +
                ", ivIndex=" + Integer.toHexString(ivIndex) +
                ", sequenceNumber=" + sequenceNumber +
                ", localAddress=" + localAddress +
                ", provisionIndex=" + provisionIndex +
                ", scenes=" + scenes.size() +
                ", groups=" + groups.size() +
                ", provisioners=" + allProvisioners.size() +
                ", provisionerNodes=" + provisionerNodes.size() +
                '}';
    }

    public String getNetKeyStr() {
        StringBuilder strBuilder = new StringBuilder();
        for (MeshNetKey meshNetKey : meshNetKeyList) {
            strBuilder.append("\nindex: ").append(meshNetKey.index).append(" -- ").append("key: ").append(Arrays.bytesToHexString(meshNetKey.key));
        }
        return strBuilder.toString();
    }

    public String getAppKeyStr() {
        StringBuilder strBuilder = new StringBuilder();
        for (MeshAppKey meshNetKey : appKeyList) {
            strBuilder.append("\nindex: ").append(meshNetKey.index).append(" -- ").append("key: ").append(Arrays.bytesToHexString(meshNetKey.key));
        }
        return strBuilder.toString();
    }

    public int getProvisionIndex() {
        return provisionIndex;
    }

    /**
     * @param addition
     */
    public void increaseProvisionIndex(int addition) {
        this.provisionIndex += addition;
        if (provisionIndex > this.addressTopLimit) {
            MeshLogger.d("");
            final int low = this.addressTopLimit + 1;
            final int high = low + 0x03FF;
            this.unicastRange.add(new AddressRange(low, high));
            this.addressTopLimit = high;
        }
        saveOrUpdate();
    }

    public void resetProvisionIndex(int index) {
        this.provisionIndex = index;
    }

    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }


    public MeshConfiguration convertToConfiguration() {
        MeshConfiguration meshConfiguration = new MeshConfiguration();
        meshConfiguration.deviceKeyMap = new SparseArray<>();
        if (nodes != null) {
            for (NodeInfo node : nodes) {
                meshConfiguration.deviceKeyMap.put(node.meshAddress, node.deviceKey);
            }
        }
        MeshNetKey netKey = getDefaultNetKey();
        meshConfiguration.netKeyIndex = netKey.index;
        meshConfiguration.networkKey = netKey.key;

        meshConfiguration.appKeyMap = new SparseArray<>();
        if (appKeyList != null) {
            for (MeshAppKey appKey :
                    appKeyList) {
                meshConfiguration.appKeyMap.put(appKey.index, appKey.key);
            }
        }

        meshConfiguration.ivIndex = ivIndex;

        meshConfiguration.sequenceNumber = sequenceNumber;

        meshConfiguration.localAddress = localAddress;

        meshConfiguration.proxyFilterWhiteList = new int[]{localAddress, MeshUtils.ADDRESS_BROADCAST};
        return meshConfiguration;
    }


    public static MeshInfo createNewMesh(Context context, String meshName) {
        // 0x7FFF

        MeshInfo meshInfo = new MeshInfo();
        if (meshName == null) {
            meshName = MESH_NAME_DEFAULT;
        }
        meshInfo.meshName = meshName;
        meshInfo.meshUUID = MeshUtils.byteArrayToUuid((MeshUtils.generateRandom(16)));
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssXXX", Locale.getDefault());
        String formattedDate = sdf.format(new Date());
        MeshLogger.d("time : " + formattedDate);
        meshInfo.timestamp = formattedDate;

//        final int IV_INDEX = 0x20345678;

        final int KEY_COUNT = 3;
        final String[] NET_KEY_NAMES = {"Default Net Key", "Sub Net Key 1", "Sub Net Key 2"};
        final String[] APP_KEY_NAMES = {"Default App Key", "Sub App Key 1", "Sub App Key 2"};
        final byte[] APP_KEY_VAL = MeshUtils.generateRandom(16);
        for (int i = 0; i < KEY_COUNT; i++) {
            meshInfo.meshNetKeyList.add(new MeshNetKey(NET_KEY_NAMES[i], i, MeshUtils.generateRandom(16)));
            meshInfo.appKeyList.add(new MeshAppKey(APP_KEY_NAMES[i],
                    i, APP_KEY_VAL, i));
        }

        meshInfo.createProvisionerInfo();
        meshInfo.ivIndex = 0x01;
        String[] groupNames = context.getResources().getStringArray(R.array.group_name);
        GroupInfo group;
        for (int i = 0; i < 8; i++) {
            group = new GroupInfo();
            group.address = i | 0xC000;
            group.name = groupNames[i];
            meshInfo.groups.add(group);
        }

        return meshInfo;
    }

    /**
     * check whether extend groups exist
     */
    public void addExtendGroups() {
        String[] extGroups = {
                " subgroup level lightness",
                " subgroup level temperature",
                " subgroup level Hue",
                " subgroup level Saturation"
        };

        int step = 0x10;

        int extGroupIdx = 0xD000;
        GroupInfo extGroup;
        if (extendGroups.size() == 0) {
            for (GroupInfo groupInfo : groups) {
                for (int i = 0; i < extGroups.length; i++) {
                    extGroup = new GroupInfo();
                    extGroup.name = groupInfo.name + extGroups[i];
                    extGroup.address = extGroupIdx + i;
                    extendGroups.add(extGroup);
                }
                extGroupIdx += step;
            }
        }
    }

    public boolean createProvisionerInfo() {
        int maxRangeHigh = 0;
        int tmpHigh;
        for (Provisioner provisioner : allProvisioners) {
            for (AddressRange unRange : provisioner.allocatedUnicastRange) {
                tmpHigh = unRange.high;
                if (maxRangeHigh == 0 || maxRangeHigh < tmpHigh) {
                    maxRangeHigh = tmpHigh;
                }
            }
        }

        int low = maxRangeHigh + 1;

        if (low + 0xFF > MeshUtils.UNICAST_ADDRESS_MAX) {
            MeshLogger.d("no available unicast range");
            return false;
        }

        int high;
        if (low == 1) {
            high = 0x03FF;
        } else {
            high = low + 0x03FF;
        }

        this.unicastRange.clear();
        this.unicastRange.add(new AddressRange(low, high));
        this.localAddress = low;
        this.resetProvisionIndex(low + 1);
        this.addressTopLimit = high;
        this.sequenceNumber = 0;
        this.ivIndex = MeshInfo.UNINITIALIZED_IVI;

        String pvUUID = SharedPreferenceHelper.getLocalUUID(TelinkMeshApplication.getInstance());
        Provisioner provisioner = new Provisioner();
        provisioner.provisionerName = PROVISIONER_NAME_DEFAULT;
        provisioner.UUID = pvUUID;
        provisioner.allocatedSceneRange.add(new SceneRange(0x01, 0x0F));
        provisioner.allocatedUnicastRange.add(new AddressRange(low, high));
        provisioner.allocatedGroupRange.add(new AddressRange(0xC000, 0xC0FF));
        this.allProvisioners.add(provisioner);

        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = this.localAddress;
        nodeInfo.name = String.format("Provisioner Node: %04X", this.localAddress);
        nodeInfo.compositionData = CompositionData.from(MeshStorageService.VC_TOOL_CPS);
        nodeInfo.elementCnt = 1;
        nodeInfo.deviceUUID = MeshUtils.uuidToByteArray(pvUUID);
        nodeInfo.bound = true;
        nodeInfo.deviceKey = Arrays.hexToBytes(NodeInfo.LOCAL_DEVICE_KEY);
        this.provisionerNodes.add(nodeInfo);
        return true;
    }

    public int getSolSeq() {
        int seq = solSeq++;
        this.saveOrUpdate();
        return seq;
    }
}

