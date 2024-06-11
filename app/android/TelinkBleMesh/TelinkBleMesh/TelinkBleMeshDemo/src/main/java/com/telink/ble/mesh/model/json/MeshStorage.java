/********************************************************************************************************
 * @file MeshStorage.java
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
package com.telink.ble.mesh.model.json;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PublishModel;
import com.telink.ble.mesh.model.db.Scheduler;
import com.telink.ble.mesh.model.db.SchedulerRegister;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * json format
 * Created by kee on 2018/9/10.
 * <p>
 * change type of period in publish from integer to object
 * add HeartbeatPublication and HeartbeatSubscription
 */

public class MeshStorage {

    interface Defaults {
        String Schema = "http://json-schema.org/draft-04/schema#";
        String Version = "1.0.0";
        String Id = "http://www.bluetooth.com/specifications/assigned-numbers/mesh-profile/cdb-schema.json#";
        String MeshName = "Telink-Sig-Mesh";

        int IV_INDEX = 0;

        String KEY_INVALID = "00000000000000000000000000000000";

        String ADDRESS_INVALID = "0000";

        String LOCAL_DEVICE_KEY = "00112233445566778899AABBCCDDEEFF";
    }

    public String $schema = Defaults.Schema;

    public String id = Defaults.Id;

    public String version = Defaults.Version;

    public String meshName = Defaults.MeshName;

    public String meshUUID;

    public String timestamp;

    public boolean partial = false;

    public List<Provisioner> provisioners;

    public List<NetworkKey> netKeys;

    public List<ApplicationKey> appKeys;

    /**
     * contains a local node (phone), its UUID is the same with provisioner uuid
     */
    public List<Node> nodes;

    public List<Group> groups;

    public List<Scene> scenes;


    /**
     * custom
     */
    // remove
//    public String ivIndex = String.format("%08X", Defaults.IV_INDEX);

    public static class Provisioner {
//        public long id;

        public String provisionerName;

        public String UUID;

        public List<AddressRange> allocatedUnicastRange;
        public List<AddressRange> allocatedGroupRange;
        public List<SceneRange> allocatedSceneRange;

        public static class AddressRange {
            public AddressRange(String lowAddress, String highAddress) {
                this.lowAddress = lowAddress;
                this.highAddress = highAddress;
            }

            public String lowAddress;
            public String highAddress;
        }

        public static class SceneRange {
            public SceneRange(String firstScene, String lastScene) {
                this.firstScene = firstScene;
                this.lastScene = lastScene;
            }

            public String firstScene;
            public String lastScene;
        }
    }

    public static class NetworkKey {
        public String name;

        // 0 -- 4095
        public int index;

        // 0,1,2
        public int phase;
        public String key;
        public String minSecurity;
        //        public String oldKey = Defaults.KEY_INVALID;
        public String timestamp;
    }

    public static class ApplicationKey {
        public String name;
        public int index;
        public int boundNetKey;
        public String key;
        public String oldKey = Defaults.KEY_INVALID;
    }

    /**
     * only contains one netKey and appKey currently
     */
    public static class Node {

        // custom: not in doc
//        public String macAddress;

        /**
         * sequence number
         * custom value
         * big endian string convert by mesh.sno
         * valued only when node is provisioner
         *
         * @see com.telink.ble.mesh.model.MeshInfo#sequenceNumber
         */
//        public String sno;

        public String UUID;
        public String unicastAddress;
        public String deviceKey;
        public String security;
        public List<NodeKey> netKeys;
        public boolean excluded = false;
        public boolean configComplete;
        public String name;
        public String cid;
        public String pid;
        public String vid;
        public String crpl;
        public Features features;
        public boolean secureNetworkBeacon = true;
        public int defaultTTL = MeshMessage.DEFAULT_TTL;
        public Transmit networkTransmit;
        public Transmit relayRetransmit;
        public List<NodeKey> appKeys;
        public List<Element> elements;
//        public boolean blacklisted; // removed in R10

        // heartbeatPub
        public HeartbeatPublication heartbeatPub;
        // heartbeatSub
        public List<HeartbeatSubscription> heartbeatSub;


        // custom data for scheduler
        public List<NodeScheduler> schedulers;
    }

    public static class NodeScheduler {
        public byte index;

        public long year;
        public long month;
        public long day;
        public long hour;
        public long minute;
        public long second;
        public long week;
        public long action;
        public long transTime;
        public int sceneId;

        public static NodeScheduler fromScheduler(Scheduler scheduler) {
            NodeScheduler nodeScheduler = new NodeScheduler();
            nodeScheduler.index = scheduler.getIndex();

            SchedulerRegister register = scheduler.register.getTarget();
            nodeScheduler.year = register.getYear();
            nodeScheduler.month = register.getMonth();
            nodeScheduler.day = register.getDay();
            nodeScheduler.hour = register.getHour();
            nodeScheduler.minute = register.getMinute();
            nodeScheduler.second = register.getSecond();
            nodeScheduler.week = register.getWeek();
            nodeScheduler.action = register.getAction();
            nodeScheduler.transTime = register.getTransTime();
            nodeScheduler.sceneId = register.getSceneId();
            return nodeScheduler;
        }
    }

    public static class Features {
        public int relay;
        public int proxy;
        public int friend;
        public int lowPower;

        public Features(int relay, int proxy, int friend, int lowPower) {
            this.relay = relay;
            this.proxy = proxy;
            this.friend = friend;
            this.lowPower = lowPower;
        }
    }

    //Network transmit && Relay retransmit
    public static class Transmit {
        // 0--7
        public int count;
        // 10--120
        public int interval;

        public Transmit(int count, int interval) {
            this.count = count;
            this.interval = interval;
        }
    }

    // node network key && node application key
    public static class NodeKey {
        public int index;
        public boolean updated;

        public NodeKey(int index, boolean updated) {
            this.index = index;
            this.updated = updated;
        }
    }

    public static class Element {
        public String name;
        public int index;
        public String location;
        public List<Model> models;
    }

    public static class Model {
        public String modelId;
        public List<String> subscribe = new ArrayList<>();
        public Publish publish;
        public List<Integer> bind;
    }

    public static class Publish {
        public String address;
        public int index;
        public int ttl;
        public PublishPeriod period;
        public int credentials;
        public Transmit retransmit;
    }

    public static class PublishPeriod {
        /**
         * The numberOfStepa property contains an integer from 0 to 63 that represents the number of steps used
         * to calculate the publish period .
         */
        public int numberOfSteps;

        /**
         * The resolution property contains an integer that represents the publish step resolution in milliseconds.
         * The allowed values are: 100, 1000, 10000, and 600000.
         */
        public int resolution;
    }

    public static class HeartbeatPublication {
        public String address;
        public int period;
        public int ttl;
        public int index;
        public List<String> features;
    }

    public static class HeartbeatSubscription {
        public String source;
        public String destination;
        public int period;
    }


    public static class Group {
        public String name;
        public String address;
        public String parentAddress = Defaults.ADDRESS_INVALID;
    }

    public static class Scene {
        public String name;
        public List<String> addresses;
        public String number;
    }

    /**
     * convert json to MeshInfo {@link MeshInfo}
     * all provisioners will be saved in provisioner list {@link MeshInfo#allProvisioners},
     * nodes will be save in three list:
     * 1. {@link MeshInfo#provisionerNodes} for all provisioner nodes
     * 2. {@link MeshInfo#excludedNodes} for all excluded nodes, that are kickout
     * 3. others nodes {@link MeshInfo#nodes}
     */
    public MeshInfo convertToMeshInfo() {
        MeshInfo meshInfo = new MeshInfo();
        meshInfo.meshUUID = meshUUID;
        meshInfo.meshName = meshName;
        meshInfo.timestamp = timestamp;
        /*
         * convert all provisioners
         */
        for (Provisioner pv : provisioners) {
            meshInfo.allProvisioners.add(com.telink.ble.mesh.model.json.Provisioner.from(pv));
        }

        /*
         * convert all network keys
         */
        for (NetworkKey networkKey : netKeys) {
            meshInfo.meshNetKeyList.add(new MeshNetKey(networkKey.name, networkKey.index, Arrays.hexToBytes(networkKey.key)));
        }

        /*
         * convert all application keys
         */
        for (ApplicationKey applicationKey : appKeys) {
            meshInfo.appKeyList.add(new MeshAppKey(applicationKey.name, applicationKey.index, Arrays.hexToBytes(applicationKey.key), applicationKey.boundNetKey));
        }


        /*
         * convert all groups
         */
        GroupInfo group;
        for (MeshStorage.Group gp : groups) {
            group = new GroupInfo();
            group.name = gp.name;
            group.address = MeshUtils.hexToIntB(gp.address);
            if (group.name.contains("subgroup")) {
                meshInfo.extendGroups.add(group);
            } else {
                meshInfo.groups.add(group);
            }
        }


        /*
         * convert all groups
         */
        com.telink.ble.mesh.model.Scene scene;
        for (MeshStorage.Scene outerScene : scenes) {
            scene = new com.telink.ble.mesh.model.Scene();
            scene.sceneId = MeshUtils.hexToIntB(outerScene.number);
            scene.name = outerScene.name;
            scene.addressList.addAll(outerScene.addresses);
            meshInfo.scenes.add(scene);
        }

        /**
         * convert all nodes,
         * the exclude nodes will be saved in meshInfo.excludedNodes,
         * other will be saved in
         * ignore provisioner nodes
         */
        for (Node node : nodes) {
            NodeInfo nodeInfo = nodeToNodeInfo(node);
//            nodeInfo.save();
            if (isProvisionerNode(node)) {
                meshInfo.provisionerNodes.add(nodeInfo);
            } else if (node.excluded) {
                meshInfo.excludedNodes.add(nodeInfo);
            } else {
                meshInfo.nodes.add(nodeInfo);
            }
        }

        return meshInfo;

    }

    /**
     * convert node to {@link NodeInfo}
     */
    private NodeInfo nodeToNodeInfo(Node node) {
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = MeshUtils.hexToIntB(node.unicastAddress);
//                    deviceInfo.deviceUUID =  Arrays.hexToBytes(node.UUID.replace(":", "").replace("-", ""));
        nodeInfo.deviceUUID = MeshUtils.uuidToByteArray(node.UUID);
        nodeInfo.name = node.name;
        nodeInfo.elementCnt = node.elements == null ? 0 : node.elements.size();
        nodeInfo.deviceKey = Arrays.hexToBytes(node.deviceKey);

        List<String> subList = new ArrayList<>();
        PublishModel publishModel;
        if (node.elements != null) {
            for (MeshStorage.Element element : node.elements) {
                if (element.models == null) {
                    continue;
                }
                for (MeshStorage.Model model : element.models) {

                    if (model.subscribe != null) {
                        for (String sub : model.subscribe) {
                            int subAdr = Integer.parseInt(sub, 16);
                            if (subAdr >= 0xD000) continue;
                            if (!MeshUtils.hexListContains(subList, sub)) {
                                subList.add(sub);
                            }
                        }
                    }
                    if (model.publish != null) {
                        MeshStorage.Publish publish = model.publish;
                        int pubAddress = MeshUtils.hexToIntB(publish.address);
                        // pub address from vc-tool， default is 0
                        if (pubAddress != 0 && publish.period != null) {
                            int elementAddress = element.index + MeshUtils.hexToIntB(node.unicastAddress);
                            int interval = (publish.retransmit.interval / 50) - 1;
                            int transmit = publish.retransmit.count | (interval << 3);
                            int periodTime = publish.period.numberOfSteps * publish.period.resolution;
                            publishModel = new PublishModel(elementAddress,
                                    MeshUtils.hexToIntB(model.modelId),
                                    MeshUtils.hexToIntB(publish.address),
                                    periodTime,
                                    publish.ttl,
                                    publish.credentials,
                                    transmit);
                            nodeInfo.setPublishModel(publishModel);
                        }

                    }
                }
            }
        }

        nodeInfo.subList = subList;
        nodeInfo.bound = (node.appKeys != null && node.appKeys.size() != 0);

        nodeInfo.compositionData = nodeToCompositionData(node);

        if (node.schedulers != null) {
            for (MeshStorage.NodeScheduler nodeScheduler : node.schedulers) {
                nodeInfo.schedulers.add(convertScheduler(nodeScheduler));
            }
        }
        return nodeInfo;
    }

    /**
     * convert node
     *
     * @param node
     * @return
     */
    public CompositionData nodeToCompositionData(Node node) {

        CompositionData compositionData = new CompositionData();

        compositionData.cid = node.cid == null || node.cid.equals("") ? 0 : MeshUtils.hexToIntB(node.cid);
        compositionData.pid = node.pid == null || node.pid.equals("") ? 0 : MeshUtils.hexToIntB(node.pid);
        compositionData.vid = node.vid == null || node.vid.equals("") ? 0 : MeshUtils.hexToIntB(node.vid);
        compositionData.crpl = node.crpl == null || node.crpl.equals("") ? 0 : MeshUtils.hexToIntB(node.crpl);

        //value 2 : unsupported
        int relaySpt = 0, proxySpt = 0, friendSpt = 0, lowPowerSpt = 0;
        if (node.features != null) {
            relaySpt = node.features.relay == 1 ? 0b0001 : 0;
            proxySpt = node.features.proxy == 1 ? 0b0010 : 0;
            friendSpt = node.features.friend == 1 ? 0b0100 : 0;
            lowPowerSpt = node.features.lowPower == 1 ? 0b1000 : 0;
        }
        compositionData.features = relaySpt | proxySpt | friendSpt | lowPowerSpt;

        compositionData.elements = new ArrayList<>();

        if (node.elements != null) {
            com.telink.ble.mesh.entity.Element infoEle;
            for (MeshStorage.Element element : node.elements) {
                infoEle = new com.telink.ble.mesh.entity.Element();

                infoEle.sigModels = new ArrayList<>();
                infoEle.vendorModels = new ArrayList<>();
                if (element.models != null && element.models.size() != 0) {
                    int modelId;
                    for (MeshStorage.Model model : element.models) {

                        // check if is vendor model
                        if (model.modelId != null && !model.modelId.equals("")) {
                            modelId = MeshUtils.hexToIntB(model.modelId);
                            // Integer.valueOf(model.modelId, 16);
                            if ((model.modelId.length()) > 4) {
                                infoEle.vendorModels.add(modelId);
                            } else {
                                infoEle.sigModels.add(modelId);
                            }
                        }

                    }
                    infoEle.sigNum = infoEle.sigModels.size();
                    infoEle.vendorNum = infoEle.vendorModels.size();
                } else {
                    infoEle.sigNum = 0;
                    infoEle.vendorNum = 0;
                }
                infoEle.location = element.location == null || element.location.equals("") ? 0 : MeshUtils.hexToIntB(element.location);
                compositionData.elements.add(infoEle);
            }
        }
        return compositionData;
    }


    /**
     * check
     *
     * @param node
     * @return
     */
    private boolean isProvisionerNode(MeshStorage.Node node) {
        for (MeshStorage.Provisioner provisioner : provisioners) {
            if (UUID.fromString(provisioner.UUID).equals(UUID.fromString(node.UUID))) {
                return true;
            }
        }
        return false;
    }


    /**
     * parse node scheduler to device scheduler
     */
    private Scheduler convertScheduler(NodeScheduler nodeScheduler) {
        return new Scheduler.Builder()
                .setIndex(nodeScheduler.index)
                .setYear((byte) nodeScheduler.year)
                .setMonth((short) nodeScheduler.month)
                .setDay((byte) nodeScheduler.day)
                .setHour((byte) nodeScheduler.hour)
                .setMinute((byte) nodeScheduler.minute)
                .setSecond((byte) nodeScheduler.second)
                .setWeek((byte) nodeScheduler.week)
                .setAction((byte) nodeScheduler.action)
                .setTransTime((byte) nodeScheduler.transTime)
                .setSceneId((short) nodeScheduler.sceneId).build();
    }


}
