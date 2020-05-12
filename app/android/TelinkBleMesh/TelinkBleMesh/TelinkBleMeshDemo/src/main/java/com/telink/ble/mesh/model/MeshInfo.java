package com.telink.ble.mesh.model;


import android.content.Context;
import android.util.SparseArray;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.NetworkLayerPDU;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.model.json.AddressRange;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import androidx.annotation.NonNull;

/**
 * Created by kee on 2019/8/22.
 */

public class MeshInfo implements Serializable, Cloneable {


    public static final String FILE_NAME = "com.telink.ble.mesh.demo.STORAGE";

    public String provisionerUUID;

    public AddressRange unicastRange;

    /**
     * nodes saved in mesh network
     */
    public List<NodeInfo> nodes = new ArrayList<>();

    /**
     * network key and network key index
     */
    public byte[] networkKey;

    public int netKeyIndex;

    /**
     * application key list
     */
    public List<AppKey> appKeyList;

//    public int appKeyIndex;


    /**
     * ivIndex and sequence number are used in NetworkPDU
     *
     * @see NetworkLayerPDU#getSeq()
     * <p>
     * should be updated and saved when {@link NetworkInfoUpdateEvent} received
     */
    public int ivIndex;

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
    public int provisionIndex = 1;

    public List<Scene> scenes = new ArrayList<>();

    public List<GroupInfo> groups = new ArrayList<>();

    public List<OOBPair> oobPairs = new ArrayList<>();

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
            if (device.getOnOff() != -1) {
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
            if (device.getOnOff() != -1) {
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


    public static class AppKey implements Serializable {
        public int index;
        public byte[] key;

        public AppKey(int index, byte[] key) {
            this.index = index;
            this.key = key;
        }
    }

    @Override
    public String toString() {
        return "MeshInfo{" +
                "nodes=" + nodes.size() +
                ", networkKey=" + Arrays.bytesToHexString(networkKey, "") +
                ", netKeyIndex=0x" + Integer.toHexString(netKeyIndex) +
                ", appKey=" + Arrays.bytesToHexString(appKeyList.get(0).key, "") +
                ", appKeyIndex=0x" + Integer.toHexString(appKeyList.get(0).index) +
                ", ivIndex=" + Integer.toHexString(ivIndex) +
                ", sequenceNumber=" + sequenceNumber +
                ", localAddress=" + localAddress +
                ", provisionIndex=" + provisionIndex +
                ", scenes=" + scenes.size() +
                ", groups=" + groups.size() +
                '}';
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

        meshConfiguration.netKeyIndex = netKeyIndex;
        meshConfiguration.networkKey = networkKey;

        meshConfiguration.appKeyMap = new SparseArray<>();
        if (appKeyList != null) {
            for (AppKey appKey :
                    appKeyList) {
                meshConfiguration.appKeyMap.put(appKey.index, appKey.key);
            }
        }

        meshConfiguration.ivIndex = ivIndex;

        meshConfiguration.sequenceNumber = sequenceNumber;

        meshConfiguration.localAddress = localAddress;

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
        meshInfo.networkKey = MeshUtils.generateRandom(16);
        meshInfo.netKeyIndex = 0x00;
        meshInfo.appKeyList = new ArrayList<>();
//        meshInfo.appKeyList.add(new MeshInfo.AppKey(0x00, APP_KEY));
        meshInfo.appKeyList.add(new AppKey(0x00, MeshUtils.generateRandom(16)));
        meshInfo.ivIndex = 0;
        meshInfo.sequenceNumber = 0;
        meshInfo.nodes = new ArrayList<>();
        meshInfo.localAddress = DEFAULT_LOCAL_ADDRESS;
        meshInfo.provisionIndex = DEFAULT_LOCAL_ADDRESS + 1; // 0x0002

        meshInfo.provisionerUUID = SharedPreferenceHelper.getLocalUUID(context);
        ;
        meshInfo.groups = new ArrayList<>();
        meshInfo.unicastRange = new AddressRange(0x01, 0xFF);
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
}

