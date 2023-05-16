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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.NetworkLayerPDU;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.model.json.AddressRange;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * Created by kee on 2019/8/22.
 */

public class MeshInfo implements Serializable, Cloneable {

    /**
     * local storage file name , saved by serialize
     */
    public static final String FILE_NAME = "com.telink.ble.mesh.demo.STORAGE";

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
    public List<AddressRange> unicastRange = new ArrayList<>();

    /**
     * nodes saved in mesh network
     */
    public List<NodeInfo> nodes = new ArrayList<>();

    /**
     * network key and network key index
     */
    public List<MeshNetKey> meshNetKeyList = new ArrayList<>();

//    public byte[] networkKey;

//    public int netKeyIndex;

    /**
     * application key list
     */
    public List<MeshAppKey> appKeyList = new ArrayList<>();

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
    public List<Scene> scenes = new ArrayList<>();

    /**
     * groups (group address 0xC000~0xC0FF)
     */
    public List<GroupInfo> groups = new ArrayList<>();

    /**
     * extend groups
     */
    public List<GroupInfo> extendGroups = new ArrayList<>();

    /**
     * static-oob info
     */
    public List<OOBPair> oobPairs = new ArrayList<>();

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

    public void insertDevice(NodeInfo deviceInfo) {
        NodeInfo local = getDeviceByUUID(deviceInfo.deviceUUID);
        if (local != null) {
            this.removeDeviceByUUID(deviceInfo.deviceUUID);
        }
        nodes.add(deviceInfo);
    }


    public boolean removeDeviceByMeshAddress(int address) {

        if (this.nodes == null || this.nodes.size() == 0) return false;

        for (Scene scene : scenes) {
            scene.removeByAddress(address);
        }

        Iterator<NodeInfo> iterator = nodes.iterator();
        while (iterator.hasNext()) {
            NodeInfo deviceInfo = iterator.next();

            if (deviceInfo.meshAddress == address) {
                iterator.remove();
                return true;
            }
        }

        return false;
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
                for (int addr : device.subList) {
                    if (addr == groupAddress) {
                        result++;
                        break;
                    }
                }
            }
        }

        return result;
    }


    public void saveScene(Scene scene) {
        for (Scene local : scenes) {
            if (local.id == scene.id) {
                local.states = scene.states;
                return;
            }
        }
        scenes.add(scene);
    }

    public Scene getSceneById(int id) {
        for (Scene scene : scenes) {
            if (id == scene.id) {
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
        int id = scenes.get(scenes.size() - 1).id;
        if (id == 0xFFFF) {
            return -1;
        }
        return id + 1;
    }

    /**
     * get oob
     */
    public byte[] getOOBByDeviceUUID(byte[] deviceUUID) {
        for (OOBPair pair : oobPairs) {
            if (Arrays.equals(pair.deviceUUID, deviceUUID)) {
                return pair.oob;
            }
        }
        return null;
    }


    public void saveOrUpdate(Context context) {
        FileSystem.writeAsObject(context, FILE_NAME, this);
    }


    @Override
    public String toString() {
        return "MeshInfo{" +
                "nodes=" + nodes.size() +
                ", netKey=" + getNetKeyStr() +
                ", appKey=" + getAppKeyStr() +
                ", ivIndex=" + Integer.toHexString(ivIndex) +
                ", sequenceNumber=" + sequenceNumber +
                ", localAddress=" + localAddress +
                ", provisionIndex=" + provisionIndex +
                ", scenes=" + scenes.size() +
                ", groups=" + groups.size() +
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


    public static MeshInfo createNewMesh(Context context) {
        // 0x7FFF
        final int DEFAULT_LOCAL_ADDRESS = 0x0001;
        MeshInfo meshInfo = new MeshInfo();

        // for test
//        final byte[] NET_KEY = Arrays.hexToBytes("26E8D2DBD4363AF398FEDE049BAD0086");

        // for test
//        final byte[] APP_KEY = Arrays.hexToBytes("7759F48730A4F1B2259B1B0681BE7C01");

//        final int IV_INDEX = 0x20345678;

//        meshInfo.networkKey = NET_KEY;
        meshInfo.meshNetKeyList = new ArrayList<>();
        final int KEY_COUNT = 3;
        final String[] NET_KEY_NAMES = {"Default Net Key", "Sub Net Key 1", "Sub Net Key 2"};
        final String[] APP_KEY_NAMES = {"Default App Key", "Sub App Key 1", "Sub App Key 2"};
        final byte[] APP_KEY_VAL = MeshUtils.generateRandom(16);
        for (int i = 0; i < KEY_COUNT; i++) {
            meshInfo.meshNetKeyList.add(new MeshNetKey(NET_KEY_NAMES[i], i, MeshUtils.generateRandom(16)));
            meshInfo.appKeyList.add(new MeshAppKey(APP_KEY_NAMES[i],
                    i, APP_KEY_VAL, i));
        }

        meshInfo.ivIndex = 0x01;
        meshInfo.sequenceNumber = 0;
        meshInfo.nodes = new ArrayList<>();
        meshInfo.localAddress = DEFAULT_LOCAL_ADDRESS;
        meshInfo.provisionIndex = DEFAULT_LOCAL_ADDRESS + 1; // 0x0002

//        meshInfo.provisionerUUID = SharedPreferenceHelper.getLocalUUID(context);
        meshInfo.provisionerUUID = MeshUtils.byteArrayToUuid((MeshUtils.generateRandom(16)));

        meshInfo.meshUUID = MeshUtils.byteArrayToUuid((MeshUtils.generateRandom(16)));


        meshInfo.groups = new ArrayList<>();
        meshInfo.unicastRange = new ArrayList<>();
        meshInfo.unicastRange.add(new AddressRange(0x01, 0x400));
        meshInfo.addressTopLimit = 0x0400;
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

}

