package com.telink.ble.mesh.model.json;

import android.text.TextUtils;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonSyntaxException;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Scheduler;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PublishModel;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MeshStorageService {
    private static MeshStorageService instance = new MeshStorageService();

    public static final String JSON_FILE = "mesh.json";

    private static final byte[] VC_TOOL_CPS = new byte[]{
            (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x01, (byte) 0x33, (byte) 0x31, (byte) 0xE8, (byte) 0x03,
            (byte) 0x04, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x17, (byte) 0x01, (byte) 0x00, (byte) 0x00,
            (byte) 0x01, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x05, (byte) 0x00,
            (byte) 0x00, (byte) 0xFE, (byte) 0x01, (byte) 0xFE, (byte) 0x02, (byte) 0xFE, (byte) 0x03, (byte) 0xFE,
            (byte) 0x00, (byte) 0xFF, (byte) 0x01, (byte) 0xFF, (byte) 0x02, (byte) 0x12, (byte) 0x01, (byte) 0x10,
            (byte) 0x03, (byte) 0x10, (byte) 0x05, (byte) 0x10, (byte) 0x08, (byte) 0x10, (byte) 0x05, (byte) 0x12,
            (byte) 0x08, (byte) 0x12, (byte) 0x02, (byte) 0x13, (byte) 0x05, (byte) 0x13, (byte) 0x09, (byte) 0x13,
            (byte) 0x11, (byte) 0x13, (byte) 0x15, (byte) 0x10, (byte) 0x11, (byte) 0x02, (byte) 0x01, (byte) 0x00
    };

    private Gson mGson;

    private MeshStorageService() {
        mGson = new GsonBuilder().setPrettyPrinting().create();
    }

    public static MeshStorageService getInstance() {
        return instance;
    }


    /**
     * import external data
     *
     * @param mesh check if outer mesh#uuid equals inner mesh#uuid
     * @return import success
     */
    public MeshInfo importExternal(String jsonStr, MeshInfo mesh) {

        MeshStorage tempStorage = null;
        try {
            tempStorage = mGson.fromJson(jsonStr, MeshStorage.class);
        } catch (JsonSyntaxException e) {
            MeshLogger.e("json import error" + e.getMessage());
        }

        if (!validStorageData(tempStorage)) {
            return null;
        }
        MeshInfo tmpMesh = null;
        try {
            tmpMesh = (MeshInfo) mesh.clone();
            if (updateLocalMesh(tempStorage, tmpMesh)) {
                return tmpMesh;
            }
        } catch (CloneNotSupportedException e) {
            e.printStackTrace();
            return null;
        }


        // sync devices
        /*mesh.devices = tempMesh.devices;
        mesh.scenes = tempMesh.scenes;
        if (!tempMesh.provisionerUUID.equals(mesh.provisionerUUID)) {
            mesh.networkKey = tempMesh.networkKey;
            mesh.netKeyIndex = tempMesh.netKeyIndex;

            mesh.appKey = tempMesh.appKey;
            mesh.appKeyIndex = tempMesh.appKeyIndex;
            AddressRange unicastRange = tempMesh.unicastRange;
            int unicastStart = unicastRange.high + 1;
            mesh.unicastRange = new AddressRange(unicastStart, unicastStart + 0xFF);
            mesh.localAddress = unicastStart;
            mesh.pvIndex = unicastStart + 1;
            mesh.ivIndex = tempMesh.ivIndex;
        } else {
            // if is the same provisioner, sync pvIndex
            mesh.pvIndex = tempMesh.pvIndex;
            mesh.sno = tempMesh.sno;
        }*/
        return null;
    }

    private boolean validStorageData(MeshStorage meshStorage) {
        return meshStorage != null && meshStorage.provisioners != null && meshStorage.provisioners.size() != 0;
    }


    /**
     * save mesh to json file
     *
     * @return file
     */
    public File exportMeshToJson(File dir, String filename, MeshInfo mesh) {
        MeshStorage meshStorage = meshToJson(mesh);
        String jsonData = mGson.toJson(meshStorage);
        return FileSystem.writeString(dir, filename, jsonData);
    }

    public String meshToJsonString(MeshInfo meshInfo) {
        MeshStorage meshStorage = meshToJson(meshInfo);
        return mGson.toJson(meshStorage);
    }

    /**
     * convert mesh instance to MeshStorage instance, for JSON export
     *
     * @param mesh instance
     */
    private MeshStorage meshToJson(MeshInfo mesh) {
        MeshStorage meshStorage = new MeshStorage();

        meshStorage.meshUUID = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
        long time = MeshUtils.getTaiTime();
        meshStorage.timestamp = String.format("%020X", time);

        // add default netKey
        MeshStorage.NetworkKey netKey = new MeshStorage.NetworkKey();
        netKey.name = "Telink Network Key";
        netKey.index = mesh.netKeyIndex;
        netKey.phase = 0;
        netKey.minSecurity = "high";
        netKey.timestamp = meshStorage.timestamp;
        netKey.key = Arrays.bytesToHexString(mesh.networkKey, "").toUpperCase();
        meshStorage.netKeys = new ArrayList<>();
        meshStorage.netKeys.add(netKey);

        // add default appKey
        MeshStorage.ApplicationKey appKey;
        meshStorage.appKeys = new ArrayList<>();
        for (MeshInfo.AppKey ak : mesh.appKeyList) {
            appKey = new MeshStorage.ApplicationKey();
            appKey.name = "Telink Application Key";
            appKey.index = ak.index;
            appKey.key = Arrays.bytesToHexString(ak.key, "").toUpperCase();

            // bound network key index
            appKey.boundNetKey = mesh.netKeyIndex;
            meshStorage.appKeys.add(appKey);
        }

        meshStorage.groups = new ArrayList<>();
//        String[] groupNames = context.getResources().getStringArray(R.array.group_name);
        List<GroupInfo> groups = mesh.groups;
        for (int i = 0; i < groups.size(); i++) {
            MeshStorage.Group group = new MeshStorage.Group();
            group.address = String.format("%04X", groups.get(i).address);
            group.name = groups.get(i).name;
            meshStorage.groups.add(group);
        }


        // create default provisioner
        MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
        provisioner.UUID = mesh.provisionerUUID;
        provisioner.provisionerName = "Telink Provisioner";
        // create uncast range, default: 0x0001 -- 0x00FF
        provisioner.allocatedUnicastRange = new ArrayList<>();
        provisioner.allocatedUnicastRange.add(
                new MeshStorage.Provisioner.AddressRange(String.format("%04X", mesh.unicastRange.low), String.format("%04X", mesh.unicastRange.high))
        );
        provisioner.allocatedGroupRange = new ArrayList<>();
        provisioner.allocatedGroupRange.add(new MeshStorage.Provisioner.AddressRange("C000", "C0FF"));
        provisioner.allocatedSceneRange = new ArrayList<>();
        provisioner.allocatedSceneRange.add(new MeshStorage.Provisioner.SceneRange(0x01, 0x0F));

        meshStorage.provisioners = new ArrayList<>();
        meshStorage.provisioners.add(provisioner);


        /**
         * create node info by provisioner info
         */
        MeshStorage.Node localNode = new MeshStorage.Node();
        // bind provisioner and node
        localNode.UUID = provisioner.UUID;
//        localNode.sno = String.format("%08X", mesh.sequenceNumber);
        localNode.unicastAddress = String.format("%04X", mesh.localAddress);
        MeshLogger.log("alloc address: " + localNode.unicastAddress);
        localNode.name = "Provisioner Node";

        // add default netKey in node
        localNode.netKeys = new ArrayList<>();
        localNode.netKeys.add(new MeshStorage.NodeKey(0, false));

        // add default appKey in node
        localNode.appKeys = new ArrayList<>();
        localNode.appKeys.add(new MeshStorage.NodeKey(0, false));
        getLocalElements(localNode, mesh.getDefaultAppKeyIndex());
        if (meshStorage.nodes == null) {
            meshStorage.nodes = new ArrayList<>();
        }
        meshStorage.nodes.add(localNode);

        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                meshStorage.nodes.add(convertDeviceInfoToNode(deviceInfo, mesh.getDefaultAppKeyIndex()));
            }
        }

        meshStorage.ivIndex = String.format("%08X", mesh.ivIndex);

        /*
         * convert [mesh.scenes] to [meshStorage.scenes]
         */
        meshStorage.scenes = new ArrayList<>();
        if (mesh.scenes != null) {
            MeshStorage.Scene scene;
            for (Scene meshScene : mesh.scenes) {
                scene = new MeshStorage.Scene();
                scene.number = meshScene.id;
                scene.name = meshScene.name;
                if (meshScene.states != null) {
                    scene.addresses = new ArrayList<>();
                    for (Scene.SceneState state : meshScene.states) {
                        scene.addresses.add(String.format("%04X", state.address));
                    }
                }
                meshStorage.scenes.add(scene);
            }
        }

        return meshStorage;
    }

    /**
     * convert meshStorage to mesh instance
     *
     * @param meshStorage imported from json object or web cloud
     * @return mesh
     */
    public boolean updateLocalMesh(MeshStorage meshStorage, MeshInfo mesh) {
//        Mesh mesh = new Mesh();

        // only import first network key
        final MeshStorage.NetworkKey networkKey = meshStorage.netKeys.get(0);
        mesh.networkKey = Arrays.hexToBytes(networkKey.key);
        mesh.netKeyIndex = networkKey.index;


        mesh.appKeyList = new ArrayList<>();
        for (MeshStorage.ApplicationKey applicationKey : meshStorage.appKeys) {
            mesh.appKeyList.add(new MeshInfo.AppKey(applicationKey.index, Arrays.hexToBytes(applicationKey.key)));
        }

//        MeshStorage.Provisioner provisioner = meshStorage.provisioners.get(0);
//        mesh.provisionerUUID = provisioner.UUID;
        if (meshStorage.provisioners == null || meshStorage.provisioners.size() == 0) {
            return false;
        }

        MeshStorage.Provisioner localProvisioner = null;
        int maxRangeHigh = -1;
        int tmpHigh;
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
            if (mesh.provisionerUUID.equals(provisioner.UUID)) {
                localProvisioner = provisioner;
                maxRangeHigh = -1;
                break;
            } else {
                for (MeshStorage.Provisioner.AddressRange unRange :
                        provisioner.allocatedUnicastRange) {
                    tmpHigh = Integer.valueOf(unRange.highAddress, 16);
                    if (maxRangeHigh == -1 || maxRangeHigh < tmpHigh) {
                        maxRangeHigh = tmpHigh;
                    }
                }
            }
        }

        /*
        if (!tempMesh.provisionerUUID.equals(mesh.provisionerUUID)) {
            mesh.networkKey = tempMesh.networkKey;
            mesh.netKeyIndex = tempMesh.netKeyIndex;

            mesh.appKey = tempMesh.appKey;
            mesh.appKeyIndex = tempMesh.appKeyIndex;
            AddressRange unicastRange = tempMesh.unicastRange;
            int unicastStart = unicastRange.high + 1;
            mesh.unicastRange = new AddressRange(unicastStart, unicastStart + 0xFF);
            mesh.localAddress = unicastStart;
            mesh.pvIndex = unicastStart + 1;
            mesh.ivIndex = tempMesh.ivIndex;
        } else {

            mesh.pvIndex = tempMesh.pvIndex;
            mesh.sno = tempMesh.sno;
        }
         */
        if (localProvisioner == null) {
            int low = maxRangeHigh + 1;
            mesh.unicastRange = new AddressRange(low, low + 0xFF);
            mesh.localAddress = low;
            mesh.provisionIndex = low + 1;
//            MeshStorage.Provisioner.AddressRange unicastRange = localProvisioner.allocatedUnicastRange.get(0);
//
//            int low = Integer.valueOf(unicastRange.lowAddress, 16);
//            int high = Integer.valueOf(unicastRange.highAddress, 16);
//            mesh.unicastRange = new AddressRange(low, high);
        }


//        mesh.groupRange = new AddressRange(0xC000, 0xC0FF);

//        mesh.sceneRange = new AddressRange(0x01, 0x0F);


        if (TextUtils.isEmpty(meshStorage.ivIndex)) {
            mesh.ivIndex = MeshStorage.Defaults.IV_INDEX;
        } else {
            mesh.ivIndex = Integer.valueOf(meshStorage.ivIndex, 16);
        }

        mesh.groups = new ArrayList<>();

        if (meshStorage.groups != null) {
            GroupInfo group;
            for (MeshStorage.Group gp : meshStorage.groups) {
                group = new GroupInfo();
                group.name = gp.name;
                group.address = Integer.valueOf(gp.address, 16);
                mesh.groups.add(group);
            }
        }

        mesh.nodes = new ArrayList<>();
        if (meshStorage.nodes != null) {
            NodeInfo deviceInfo;
            for (MeshStorage.Node node : meshStorage.nodes) {
                if (!isProvisionerNode(meshStorage, node)) {
                    deviceInfo = new NodeInfo();
                    deviceInfo.meshAddress = Integer.valueOf(node.unicastAddress, 16);
                    deviceInfo.deviceUUID = (Arrays.hexToBytes(node.UUID.replace(":", "").replace("-", "")));
                    deviceInfo.elementCnt = node.elements == null ? 0 : node.elements.size();
                    deviceInfo.deviceKey = Arrays.hexToBytes(node.deviceKey);

                    List<Integer> subList = new ArrayList<>();
                    PublishModel publishModel;
                    if (node.elements != null) {
                        for (MeshStorage.Element element : node.elements) {
                            if (element.models == null) {
                                continue;
                            }
                            for (MeshStorage.Model model : element.models) {

                                if (model.subscribe != null) {
                                    int subAdr;
                                    for (String sub : model.subscribe) {
                                        subAdr = Integer.valueOf(sub, 16);
                                        if (!subList.contains(subAdr)) {
                                            subList.add(subAdr);
                                        }
                                    }
                                }
                                if (model.publish != null) {
                                    MeshStorage.Publish publish = model.publish;
                                    int pubAddress = Integer.valueOf(publish.address, 16);
                                    // pub address from vc-tool， default is 0
                                    if (pubAddress != 0 && publish.period != 0) {
                                        int elementAddress = element.index + Integer.valueOf(node.unicastAddress, 16);
                                        int transmit = publish.retransmit.count | (publish.retransmit.interval << 3);
                                        publishModel = new PublishModel(elementAddress,
                                                Integer.valueOf(model.modelId, 16),
                                                Integer.valueOf(publish.address, 16),
                                                publish.period,
                                                publish.ttl,
                                                publish.credentials,
                                                transmit);
                                        deviceInfo.setPublishModel(publishModel);
                                    }

                                }
                            }
                        }
                    }


                    deviceInfo.subList = subList;
//                    deviceInfo.setPublishModel();
                    deviceInfo.state = (node.appKeys != null && node.appKeys.size() != 0)
                            ? NodeInfo.STATE_BIND_SUCCESS : NodeInfo.STATE_BIND_FAIL;

                    deviceInfo.compositionData = convertNodeToNodeInfo(node);

                    if (node.schedulers != null) {
                        deviceInfo.schedulers = new ArrayList<>();
                        for (MeshStorage.NodeScheduler nodeScheduler : node.schedulers) {
                            deviceInfo.schedulers.add(parseNodeScheduler(nodeScheduler));
                        }
                    }

                    mesh.nodes.add(deviceInfo);
                } else {
//                    mesh.localAddress = Integer.valueOf(node.unicastAddress, 16);

                }
            }
        }

        mesh.scenes = new ArrayList<>();
        if (meshStorage.scenes != null && meshStorage.scenes.size() != 0) {
            Scene scene;
            for (MeshStorage.Scene outerScene : meshStorage.scenes) {
                scene = new Scene();
                scene.id = outerScene.number;
                scene.name = outerScene.name;
                if (outerScene.addresses != null) {
                    scene.states = new ArrayList<>(outerScene.addresses.size());
                    for (String adrInScene : outerScene.addresses) {
                        // import scene state
                        scene.states.add(new Scene.SceneState(Integer.valueOf(adrInScene, 16)));
                    }
                }
                mesh.scenes.add(scene);
            }
        }
        return true;
    }


    // convert nodeInfo(mesh.java) to node(json)
    public MeshStorage.Node convertDeviceInfoToNode(NodeInfo deviceInfo, int appKeyIndex) {
        MeshStorage.Node node = new MeshStorage.Node();
        node.UUID = Arrays.bytesToHexString(deviceInfo.deviceUUID).toUpperCase();
        node.unicastAddress = String.format("%04X", deviceInfo.meshAddress);

        if (deviceInfo.deviceKey != null) {
            node.deviceKey = Arrays.bytesToHexString(deviceInfo.deviceKey, "").toUpperCase();
        }
        node.elements = new ArrayList<>(deviceInfo.elementCnt);

        if (deviceInfo.compositionData != null) {
            node.deviceKey = Arrays.bytesToHexString(deviceInfo.deviceKey, "").toUpperCase();
            node.cid = String.format("%04X", deviceInfo.compositionData.cid);
            node.pid = String.format("%04X", deviceInfo.compositionData.pid);
            node.vid = String.format("%04X", deviceInfo.compositionData.vid);
            node.crpl = String.format("%04X", deviceInfo.compositionData.crpl);
            int features = deviceInfo.compositionData.features;
            // value in supported node is 1, value in unsupported node is 0 (as 2 in json)
            // closed
            /*node.features = new MeshStorage.Features((features & 0b0001) == 0 ? 2 : 1,
                    (features & 0b0010) == 0 ? 2 : 1,
                    (features & 0b0100) == 0 ? 2 : 1,
                    (features & 0b1000) == 0 ? 2 : 1);*/
            node.features = new MeshStorage.Features(features & 0b0001,
                    features & 0b0010,
                    features & 0b0100,
                    features & 0b1000);


            PublishModel publishModel = deviceInfo.getPublishModel();

            if (deviceInfo.compositionData.elements != null) {
                List<CompositionData.Element> elements = deviceInfo.compositionData.elements;
                MeshStorage.Element element;
                for (int i = 0; i < elements.size(); i++) {
                    CompositionData.Element ele = elements.get(i);
                    element = new MeshStorage.Element();
                    element.index = i;
                    element.location = String.format("%04X", ele.location);

                    element.models = new ArrayList<>();
                    MeshStorage.Model model;

                    if (ele.sigNum != 0 && ele.sigModels != null) {
                        for (int modelId : ele.sigModels) {
                            model = new MeshStorage.Model();
                            model.modelId = String.format("%04X", modelId);
                            model.bind = new ArrayList<>();
                            model.bind.add(appKeyIndex);

                            model.subscribe = new ArrayList<>();
                            if (inDefaultSubModel(modelId)) {
                                for (int subAdr : deviceInfo.subList) {
                                    model.subscribe.add(String.format("%04X", subAdr));
                                }
                            }

                            if (publishModel != null && publishModel.modelId == modelId) {
                                final MeshStorage.Publish publish = new MeshStorage.Publish();
                                publish.address = String.format("%04X", publishModel.address);
                                publish.index = 0;

                                publish.ttl = publishModel.ttl;
                                publish.period = publishModel.period;
                                publish.credentials = publishModel.credential;
                                publish.retransmit = new MeshStorage.Transmit(publishModel.getTransmitCount()
                                        , publishModel.getTransmitInterval());

                                model.publish = publish;
                            }

                            element.models.add(model);
                        }
                    }

                    if (ele.vendorNum != 0 && ele.vendorModels != null) {

                        for (int modelId : ele.vendorModels) {
                            model = new MeshStorage.Model();
                            model.modelId = String.format("%08X", modelId);
                            model.bind = new ArrayList<>();
                            model.bind.add(appKeyIndex);
                            element.models.add(model);
                        }
                    }
                    node.elements.add(element);
                }
            }
        } else {

            // create elements
            for (int i = 0; i < deviceInfo.elementCnt; i++) {
                node.elements.add(new MeshStorage.Element());
            }
        }
        node.netKeys = new ArrayList<>();
        node.netKeys.add(new MeshStorage.NodeKey(0, false));
        node.configComplete = true;
        node.name = "Common Node";

        // check if appKey list exists to confirm device bound state
        if (deviceInfo.state == NodeInfo.STATE_BIND_SUCCESS) {
            node.appKeys = new ArrayList<>();
            node.appKeys.add(new MeshStorage.NodeKey(0, false));
        }

        node.security = MeshSecurity.High.getDesc();


        if (deviceInfo.schedulers != null) {
            node.schedulers = new ArrayList<>();
            for (Scheduler deviceScheduler : deviceInfo.schedulers) {
                node.schedulers.add(MeshStorage.NodeScheduler.fromScheduler(deviceScheduler));
            }
        }

        return node;
    }

    private void getLocalElements(MeshStorage.Node node, int appKeyIndex) {

        node.elements = new ArrayList<>();
        CompositionData compositionData = CompositionData.from(VC_TOOL_CPS);

        List<CompositionData.Element> elements = compositionData.elements;
        MeshStorage.Element element;
        for (int i = 0; i < elements.size(); i++) {
            CompositionData.Element ele = elements.get(i);
            element = new MeshStorage.Element();
            element.index = i;
            element.location = String.format("%04X", ele.location);

            element.models = new ArrayList<>();
            MeshStorage.Model model;

            if (ele.sigNum != 0 && ele.sigModels != null) {
                for (int modelId : ele.sigModels) {
                    model = new MeshStorage.Model();
                    model.modelId = String.format("%04X", modelId);
                    model.bind = new ArrayList<>();
                    model.bind.add(appKeyIndex);

                    model.subscribe = new ArrayList<>();

                    element.models.add(model);
                }
            }

            if (ele.vendorNum != 0 && ele.vendorModels != null) {

                for (int modelId : ele.vendorModels) {
                    model = new MeshStorage.Model();
                    model.modelId = String.format("%08X", modelId);
                    model.bind = new ArrayList<>();
                    model.bind.add(appKeyIndex);
                    element.models.add(model);
                }
            }
            node.elements.add(element);
        }
    }

    //
    private boolean inDefaultSubModel(int modelId) {
        MeshSigModel[] models = MeshSigModel.getDefaultSubList();
        for (MeshSigModel model : models) {
            if (model.modelId == modelId) {
                return true;
            }
        }
        return false;
    }


    /**
     * convert node in json to composition data
     */
    public CompositionData convertNodeToNodeInfo(MeshStorage.Node node) {

        CompositionData compositionData = new CompositionData();

        compositionData.cid = node.cid == null || node.cid.equals("") ? 0 : Integer.valueOf(node.cid, 16);
        compositionData.pid = node.pid == null || node.pid.equals("") ? 0 : Integer.valueOf(node.pid, 16);
        compositionData.vid = node.vid == null || node.vid.equals("") ? 0 : Integer.valueOf(node.vid, 16);
        compositionData.crpl = node.crpl == null || node.crpl.equals("") ? 0 : Integer.valueOf(node.crpl, 16);

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
            CompositionData.Element infoEle;
            for (MeshStorage.Element element : node.elements) {
                infoEle = new CompositionData.Element();

                infoEle.sigModels = new ArrayList<>();
                infoEle.vendorModels = new ArrayList<>();
                if (element.models != null && element.models.size() != 0) {
                    int modelId;
                    for (MeshStorage.Model model : element.models) {

                        // check if is vendor model
                        if (model.modelId != null && !model.modelId.equals("")) {
                            modelId = Integer.valueOf(model.modelId, 16);
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
                infoEle.location = element.location == null || element.location.equals("") ? 0 : Integer.valueOf(element.location, 16);
                compositionData.elements.add(infoEle);
            }
        }
        return compositionData;
    }


    // check if node is provisioner
    private boolean isProvisionerNode(MeshStorage meshStorage, MeshStorage.Node node) {
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
            if (provisioner.UUID.equals(node.UUID)) {
                return true;
            }
        }
        return false;
    }

    /**
     * parse node scheduler to device scheduler
     */
    private Scheduler parseNodeScheduler(MeshStorage.NodeScheduler nodeScheduler) {
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
