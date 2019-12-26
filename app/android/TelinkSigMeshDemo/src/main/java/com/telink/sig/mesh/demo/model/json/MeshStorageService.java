package com.telink.sig.mesh.demo.model.json;

import android.text.TextUtils;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonSyntaxException;
import com.telink.sig.mesh.demo.FileSystem;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.AddressRange;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.Scene;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.PublishModel;
import com.telink.sig.mesh.model.Scheduler;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.io.File;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;

/**
 * json 分享机制： 可以通过手机1 加灯后，把文件拷贝给手机2， 手机2会判断UUID不一致， 生成新的provisioner 并添加地址区间,
 * 然后从该地址区间alloc mesh address 给provisioner对应的node
 * 手机2 加灯的信息， 需要将文件拷回至手机1 才可以同步; 手机1在拷贝完成时， 会判断到UUID已存在， 只需要merge对应信息即可；
 */
public class MeshStorageService {
    private static MeshStorageService instance = new MeshStorageService();

    public static final String JSON_FILE = "mesh.json";

    private static final int UNICAST_ADDRESS_MAX = 0b0111111111111111;

    private static final int GROUP_ADDRESS_MAX = 0xFEFF;

    private static final int SCENE_ID_MAX = 0xFF;

//    private MeshStorage meshStorage;

    // 本地的provisioner
//    private MeshStorage.Provisioner localProvisioner;

    private Gson mGson;

    private MeshStorageService() {
        mGson = new GsonBuilder().excludeFieldsWithModifiers(Modifier.PRIVATE)
                .setPrettyPrinting().create();
    }

    public static MeshStorageService getInstance() {
        return instance;
    }


    /**
     * import external data
     *
     * @param filepath data file path
     * @param mesh     check if outer mesh#uuid equals inner mesh#uuid
     * @return import success
     */
    public Mesh importExternal(String filepath, Mesh mesh) {

        File file = new File(filepath);
        if (!file.exists()) {
            return mesh;
        }

        String fileAsStr = FileSystem.readString(file);
        MeshStorage tempStorage = null;
        try {
            tempStorage = mGson.fromJson(fileAsStr, MeshStorage.class);
        } catch (JsonSyntaxException e) {
            TelinkLog.e("json import error" + e.getMessage());
        }

        if (!validStorageData(tempStorage)) {
            return mesh;
        }
        Mesh tmpMesh = null;
        try {
            tmpMesh = (Mesh) mesh.clone();
            if (updateLocalMesh(tempStorage, tmpMesh)) {
                return tmpMesh;
            }
        } catch (CloneNotSupportedException e) {
            e.printStackTrace();
        }


        // 设备信息 必须要同步
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
            // 在provisioner 相同时， 只需要把pv index同步即可
            mesh.pvIndex = tempMesh.pvIndex;
            mesh.sno = tempMesh.sno;
        }*/
        return mesh;
    }

    private boolean validStorageData(MeshStorage meshStorage) {
        return meshStorage != null && meshStorage.provisioners != null && meshStorage.provisioners.size() != 0;
    }


    /**
     * save mesh to json file
     *
     * @return file
     */
    public File saveAs(File dir, String filename, Mesh mesh) {
        MeshStorage meshStorage = meshToJson(mesh);
        String jsonData = mGson.toJson(meshStorage);
        return FileSystem.writeString(dir, filename, jsonData);
    }

    /**
     * export
     * convert mesh instance to MeshStorage instance, for JSON export
     *
     * @param mesh instance
     */
    private MeshStorage meshToJson(Mesh mesh) {
        MeshStorage meshStorage = new MeshStorage();

        // update: compare meshUuid with mesh.uuid
//        meshStorage.meshUUID = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
        meshStorage.meshUUID = mesh.uuid;

        long time = MeshUtils.getTaiTime();
        meshStorage.timestamp = String.format("%020X", time);

        // 添加默认的netKey
        MeshStorage.NetworkKey netKey = new MeshStorage.NetworkKey();
        netKey.name = "Telink Network Key";
        netKey.index = mesh.netKeyIndex;
        netKey.phase = 0;
        netKey.minSecurity = "high";
        netKey.timestamp = meshStorage.timestamp;
        netKey.key = Arrays.bytesToHexString(mesh.networkKey, "").toUpperCase();
        meshStorage.netKeys = new ArrayList<>();
        meshStorage.netKeys.add(netKey);

        // 添加默认的appKey
        MeshStorage.ApplicationKey appKey = new MeshStorage.ApplicationKey();
        appKey.name = "Telink Application Key";
        appKey.index = mesh.appKeyIndex;
        appKey.key = Arrays.bytesToHexString(mesh.appKey, "").toUpperCase();
        // 绑定的network key index，即所在network
        appKey.boundNetKey = 0;
        meshStorage.appKeys = new ArrayList<>();
        meshStorage.appKeys.add(appKey);

        meshStorage.groups = new ArrayList<>();
//        String[] groupNames = context.getResources().getStringArray(R.array.group_name);
        List<Group> groups = mesh.groups;
        for (int i = 0; i < groups.size(); i++) {
            MeshStorage.Group group = new MeshStorage.Group();
            group.address = String.format("%04X", groups.get(i).address);
            group.name = groups.get(i).name;
            meshStorage.groups.add(group);
        }

        meshStorage.provisioners = new ArrayList<>();
        meshStorage.nodes = new ArrayList<>();


        // add provisioner and provisioner-node
        if (mesh.provisionerNodes == null || mesh.provisionerNodes.size() == 0) {
            // local created mesh
            // 创建默认provisioner数据
            MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
            provisioner.UUID = mesh.provisionerUUID;
            provisioner.provisionerName = "Telink Provisioner";
            // 创建 可分配的设备unicast地址列表 0x0001 -- 0x00FF
            provisioner.allocatedUnicastRange = new ArrayList<>();
            provisioner.allocatedUnicastRange.add(
                    new MeshStorage.Provisioner.AddressRange(String.format("%04X", mesh.unicastRange.low), String.format("%04X", mesh.unicastRange.high))
            );
            provisioner.allocatedGroupRange = new ArrayList<>();
            provisioner.allocatedGroupRange.add(new MeshStorage.Provisioner.AddressRange("C000", "C0FF"));
            provisioner.allocatedSceneRange = new ArrayList<>();
            provisioner.allocatedSceneRange.add(new MeshStorage.Provisioner.SceneRange(0x01, 0x0F));


            meshStorage.provisioners.add(provisioner);

            /**
             * 创建与provisioner对应的node信息
             */
            MeshStorage.Node localNode = new MeshStorage.Node();
            // 关联 provisioner 和 node 信息
            localNode.UUID = provisioner.UUID;
//            localNode.sno = String.format("%08X", mesh.sno);
            localNode.unicastAddress = String.format("%04X", mesh.localAddress);
            TelinkLog.d("alloc address: " + localNode.unicastAddress);
            localNode.name = "Provisioner Node";

            // 添加默认的netKey到node
            localNode.netKeys = new ArrayList<>();
            localNode.netKeys.add(new MeshStorage.NodeKey(0, false));

            // 添加默认的appKey到node
            localNode.appKeys = new ArrayList<>();
            localNode.appKeys.add(new MeshStorage.NodeKey(0, false));
            getLocalElements(localNode, mesh.appKeyIndex);

            meshStorage.nodes.add(localNode);
        } else {

            for (ProvisionerNode provisionerNode :
                    mesh.provisionerNodes) {

                MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
                provisioner.UUID = provisionerNode.provisioner.UUID;
                provisioner.provisionerName = "Telink Provisioner";
                // 创建 可分配的设备unicast地址列表 0x0001 -- 0x00FF
                provisioner.allocatedUnicastRange = provisionerNode.provisioner.allocatedUnicastRange;
                provisioner.allocatedGroupRange = provisionerNode.provisioner.allocatedGroupRange;
                provisioner.allocatedSceneRange = provisionerNode.provisioner.allocatedSceneRange;
                meshStorage.provisioners.add(provisioner);

                // imported from others
                meshStorage.nodes.add(provisionerNode.node);

            }


        }

        // add mesh nodes
        if (mesh.devices != null) {
            for (DeviceInfo deviceInfo : mesh.devices) {
                meshStorage.nodes.add(convertDeviceInfoToNode(deviceInfo, mesh.appKeyIndex));
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
    public boolean updateLocalMesh(MeshStorage meshStorage, Mesh mesh) {
//        Mesh mesh = new Mesh();

        // only import first network key
        final MeshStorage.NetworkKey networkKey = meshStorage.netKeys.get(0);
        mesh.networkKey = Arrays.hexToBytes(networkKey.key);
        mesh.netKeyIndex = networkKey.index;

        // only import first app key
        final MeshStorage.ApplicationKey appKey = meshStorage.appKeys.get(0);
        mesh.appKey = Arrays.hexToBytes(appKey.key);
        mesh.appKeyIndex = appKey.index;


//        MeshStorage.Provisioner provisioner = meshStorage.provisioners.get(0);
//        mesh.provisionerUUID = provisioner.UUID;
        if (meshStorage.provisioners == null || meshStorage.provisioners.size() == 0) {
            return false;
        }

        MeshStorage.Provisioner localProvisioner = null;
        int maxRangeHigh = -1;
        int tmpHigh;
        mesh.provisionerNodes = new ArrayList<>();
        outer:
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {

            // local provisioner
            if (mesh.provisionerUUID.equals(provisioner.UUID)) {
                if (mesh.uuid.equals(meshStorage.meshUUID)) {
                    // use local provisioner info
                } else {
                    mesh.localAddress = ++mesh.pvIndex;
                }

                localProvisioner = provisioner;
                maxRangeHigh = -1;

            } else {
                // calculate max range
                for (MeshStorage.Provisioner.AddressRange unRange :
                        provisioner.allocatedUnicastRange) {
                    tmpHigh = Integer.valueOf(unRange.highAddress, 16);
                    if (maxRangeHigh == -1 || maxRangeHigh < tmpHigh) {
                        maxRangeHigh = tmpHigh;
                    }
                }
            }


            // find provisioner node
            for (MeshStorage.Node node : meshStorage.nodes) {
                if (provisioner.UUID.equals(node.UUID)) {
                    ProvisionerNode provisionerNode = new ProvisionerNode();
                    provisionerNode.provisioner = provisioner;
                    provisionerNode.node = node;
                    mesh.provisionerNodes.add(provisionerNode);
                    continue outer;
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
            // 在provisioner 相同时， 只需要把pv index同步即可
            mesh.pvIndex = tempMesh.pvIndex;
            mesh.sno = tempMesh.sno;
        }
         */
        if (localProvisioner == null) {
            int low = maxRangeHigh + 1;
            mesh.unicastRange = new AddressRange(low, low + 0x03FF);
            mesh.localAddress = low;
            mesh.pvIndex = low + 1;
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
            Group group;
            for (MeshStorage.Group gp : meshStorage.groups) {
                group = new Group();
                group.name = gp.name;
                group.address = Integer.valueOf(gp.address, 16);
                mesh.groups.add(group);
            }
        }

        mesh.devices = new ArrayList<>();
        if (meshStorage.nodes != null) {
            DeviceInfo deviceInfo;
            for (MeshStorage.Node node : meshStorage.nodes) {
                if (!isProvisionerNode(meshStorage, node)) {
                    deviceInfo = new DeviceInfo();
                    deviceInfo.meshAddress = Integer.valueOf(node.unicastAddress, 16);
                    deviceInfo.macAddress = Arrays.bytesToHexString(Arrays.hexToBytes(node.macAddress.replace(":", "")), ":").toUpperCase();
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
                                    // vc 传过来的 pub address， 在没有配置是为0
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
                    deviceInfo.bindState = (node.appKeys != null && node.appKeys.size() != 0)
                            ? DeviceBindState.BOUND : DeviceBindState.UNBIND;

                    deviceInfo.nodeInfo = convertNodeToNodeInfo(node);

                    if (node.schedulers != null) {
                        deviceInfo.schedulers = new ArrayList<>();
                        for (MeshStorage.NodeScheduler nodeScheduler : node.schedulers) {
                            deviceInfo.schedulers.add(parseNodeScheduler(nodeScheduler));
                        }
                    }

                    mesh.devices.add(deviceInfo);
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


    // mesh中的deviceInfo 转成 json 中的 node
    public MeshStorage.Node convertDeviceInfoToNode(DeviceInfo deviceInfo, int appKeyIndex) {
        MeshStorage.Node node = new MeshStorage.Node();
        node.macAddress = deviceInfo.macAddress.replace(":", "").toUpperCase();
        node.unicastAddress = String.format("%04X", deviceInfo.meshAddress);

        if (deviceInfo.deviceKey != null) {
            node.deviceKey = Arrays.bytesToHexString(deviceInfo.deviceKey, "").toUpperCase();
        }
        node.elements = new ArrayList<>(deviceInfo.elementCnt);

        if (deviceInfo.nodeInfo != null) {
            node.deviceKey = Arrays.bytesToHexString(deviceInfo.nodeInfo.deviceKey, "").toUpperCase();
            node.cid = String.format("%04X", deviceInfo.nodeInfo.cpsData.cid);
            node.pid = String.format("%04X", deviceInfo.nodeInfo.cpsData.pid);
            node.vid = String.format("%04X", deviceInfo.nodeInfo.cpsData.vid);
            node.crpl = String.format("%04X", deviceInfo.nodeInfo.cpsData.crpl);
            int features = deviceInfo.nodeInfo.cpsData.features;
            // 支持的节点默认都是打开的， 即为1， 不支持的节点 composition 数据是0， 传到 node 中为2
            // 已关闭相关判断
            /*node.features = new MeshStorage.Features((features & 0b0001) == 0 ? 2 : 1,
                    (features & 0b0010) == 0 ? 2 : 1,
                    (features & 0b0100) == 0 ? 2 : 1,
                    (features & 0b1000) == 0 ? 2 : 1);*/
            node.features = new MeshStorage.Features(features & 0b0001,
                    features & 0b0010,
                    features & 0b0100,
                    features & 0b1000);


            PublishModel publishModel = deviceInfo.getPublishModel();

            if (deviceInfo.nodeInfo.cpsData.elements != null) {
                List<NodeInfo.Element> elements = deviceInfo.nodeInfo.cpsData.elements;
                MeshStorage.Element element;
                for (int i = 0; i < elements.size(); i++) {
                    NodeInfo.Element ele = elements.get(i);
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

            // 创建空的element对象， 用于同步element个数
            for (int i = 0; i < deviceInfo.elementCnt; i++) {
                node.elements.add(new MeshStorage.Element());
            }
        }
        node.netKeys = new ArrayList<>();
        node.netKeys.add(new MeshStorage.NodeKey(0, false));
        node.configComplete = true;
        node.name = "Common Node";

        //目前根据appKey列表是否存在判断设备是否绑定成功
        if (deviceInfo.bindState == DeviceBindState.BOUND) {
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
        byte[] cps = TelinkMeshApplication.getInstance().getMeshLib().getLocalCpsData();
        NodeInfo.CompositionData compositionData = convertLocal(cps);

        List<NodeInfo.Element> elements = compositionData.elements;
        MeshStorage.Element element;
        for (int i = 0; i < elements.size(); i++) {
            NodeInfo.Element ele = elements.get(i);
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


    public NodeInfo.CompositionData convertLocal(byte[] data) {
        int index = 0;
        NodeInfo.CompositionData cpsData = new NodeInfo.CompositionData();
        cpsData.cid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.pid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.vid = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.crpl = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
        cpsData.features = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);

        cpsData.elements = new ArrayList<>();
        while (index < data.length) {
            NodeInfo.Element element = new NodeInfo.Element();
            element.location = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            element.sigNum = (data[index++] & 0xFF);
            element.vendorNum = (data[index++] & 0xFF);

            element.sigModels = new ArrayList<>();
            for (int i = 0; i < element.sigNum; i++) {
                element.sigModels.add((data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8));
            }

            element.vendorModels = new ArrayList<>();
            for (int j = 0; j < element.vendorNum; j++) {
                //sample 11 02 01 00 cid: 11 02 modelId: 01 00 -> 0x00010211
                element.vendorModels.add(((data[index++] & 0xFF)) | ((data[index++] & 0xFF) << 8) |
                        ((data[index++] & 0xFF) << 16) | ((data[index++] & 0xFF) << 24));
            }

            cpsData.elements.add(element);
            break;
        }

        return cpsData;
    }


    //
    private boolean inDefaultSubModel(int modelId) {
        SigMeshModel[] models = SigMeshModel.getDefaultSubList();
        for (SigMeshModel model : models) {
            if (model.modelId == modelId) {
                return true;
            }
        }
        return false;
    }


    /**
     * 把json格式的 node 信息转为 node info
     */
    public NodeInfo convertNodeToNodeInfo(MeshStorage.Node node) {
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.nodeAdr = Integer.valueOf(node.unicastAddress, 16);
        nodeInfo.elementCnt = node.elements == null ? 0 : node.elements.size();
        nodeInfo.rsv = 0xFF;

        nodeInfo.deviceKey = node.deviceKey == null || node.deviceKey.equals("") ? null : Arrays.hexToBytes(node.deviceKey);

        nodeInfo.cpsData = new NodeInfo.CompositionData();

        nodeInfo.cpsData.cid = node.cid == null || node.cid.equals("") ? 0 : Integer.valueOf(node.cid, 16);
        nodeInfo.cpsData.pid = node.pid == null || node.pid.equals("") ? 0 : Integer.valueOf(node.pid, 16);
        nodeInfo.cpsData.vid = node.vid == null || node.vid.equals("") ? 0 : Integer.valueOf(node.vid, 16);
        nodeInfo.cpsData.crpl = node.crpl == null || node.crpl.equals("") ? 0 : Integer.valueOf(node.crpl, 16);

        //value 2 : unsupported
        int relaySpt = 0, proxySpt = 0, friendSpt = 0, lowPowerSpt = 0;
        if (node.features != null) {
            relaySpt = node.features.relay == 1 ? 0b0001 : 0;
            proxySpt = node.features.proxy == 1 ? 0b0010 : 0;
            friendSpt = node.features.friend == 1 ? 0b0100 : 0;
            lowPowerSpt = node.features.lowPower == 1 ? 0b1000 : 0;
        }
        nodeInfo.cpsData.features = relaySpt | proxySpt | friendSpt | lowPowerSpt;


        nodeInfo.cpsData.elements = new ArrayList<>(nodeInfo.elementCnt);


        if (node.elements != null) {
            NodeInfo.Element infoEle;
            for (MeshStorage.Element element : node.elements) {
                infoEle = new NodeInfo.Element();

                infoEle.sigModels = new ArrayList<>();
                infoEle.vendorModels = new ArrayList<>();
                if (element.models != null && element.models.size() != 0) {
                    int modelId;
                    for (MeshStorage.Model model : element.models) {

                        // 根据model id字节数判断是否为vendor model
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
                nodeInfo.cpsData.elements.add(infoEle);
            }
        }
        return nodeInfo;
    }


    // 判断node是否是 provisioner节点
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
