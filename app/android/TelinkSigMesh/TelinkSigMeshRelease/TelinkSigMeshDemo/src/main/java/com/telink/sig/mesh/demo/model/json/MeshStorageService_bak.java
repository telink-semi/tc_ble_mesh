package com.telink.sig.mesh.demo.model.json;

import android.content.Context;
import android.text.TextUtils;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonSyntaxException;
import com.telink.sig.mesh.demo.FileSystem;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.PublishModel;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;

/**
 * Created by kee on 2018/12/14.
 */

/**
 * json 分享机制： 可以通过手机1 加灯后，把文件拷贝给手机2， 手机2会判断UUID不一致， 生成新的provisioner 并添加地址区间,
 * 然后从该地址区间alloc mesh address 给provisioner对应的node
 * 手机2 加灯的信息， 需要将文件拷回至手机1 才可以同步; 手机1在拷贝完成时， 会判断到UUID已存在， 只需要merge对应信息即可；
 */
public class MeshStorageService_bak {
    private static MeshStorageService_bak instance = new MeshStorageService_bak();

    public static final String JSON_FILE = "mesh.json";

    private static final int UNICAST_ADDRESS_MAX = 0b0111111111111111;

    private static final int GROUP_ADDRESS_MAX = 0xFEFF;

    private static final int SCENE_ID_MAX = 0xFF;

    private MeshStorage meshStorage;

    // 本地的provisioner
    private MeshStorage.Provisioner localProvisioner;

    private Gson mGson;

    private MeshStorageService_bak() {
//        mGson = new Gson();
        mGson = new GsonBuilder().setPrettyPrinting().create();
    }

    public static MeshStorageService_bak getInstance() {
        return instance;
    }


    public File getFilePath(Context context) {
        return new File(context.getFilesDir(), JSON_FILE);
    }

    // 加载 json 文件
    // 判断json 文件中是否 有本地的provsioner信息， 如果没有， 说明是从外部导入的
    public void load(Context context) {
        meshStorage = null;
        String jsonData = FileSystem.readString(getFilePath(context));
        if (!jsonData.equals("")) {
            try {
                meshStorage = mGson.fromJson(jsonData, MeshStorage.class);
            } catch (JsonSyntaxException e) {
                TelinkLog.e("json import error" + e.getMessage());
            }

        }
        if (meshStorage == null) {
            clearAndRecreate(context);
            this.save(context);
        } else {
            final String localUUID = TelinkMeshApplication.getInstance().getLocalUUID();
            boolean contains = false;
            if (meshStorage.provisioners != null) {
                for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
                    if (localUUID.equals(provisioner.UUID)) {
                        localProvisioner = provisioner;
                        contains = true;
                        break;
                    }
                }
            }
            if (!contains) {
                createAndInsertProvisioner();
            }
        }
    }

    /**
     * import external data
     *
     * @param filepath data file path
     * @return import success
     */
    public synchronized boolean importExternal(String filepath) {
        File file = new File(filepath);
        if (!file.exists()) {
            return false;
        }

        String fileAsStr = FileSystem.readString(file);
        MeshStorage tempStorage = null;
        try {
            tempStorage = mGson.fromJson(fileAsStr, MeshStorage.class);
        } catch (JsonSyntaxException e) {
            TelinkLog.e("json import error" + e.getMessage());
        }

        if (!validStorageData(tempStorage)) {
            return false;
        }

        meshStorage = tempStorage;
        final String localUUID = TelinkMeshApplication.getInstance().getLocalUUID();
        boolean contains = false;
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
            if (provisioner.UUID != null && provisioner.UUID.equals(localUUID)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            createAndInsertProvisioner();
        }

        return true;
    }

    private boolean validStorageData(MeshStorage meshStorage) {
        return meshStorage != null && meshStorage.provisioners != null && meshStorage.provisioners.size() != 0;
    }

    // 保存
    public void save(Context context) {
        setTimestamp();
        String jsonData = mGson.toJson(meshStorage);
        FileSystem.writeString(context.getFilesDir(), JSON_FILE, jsonData);
        /*Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        Uri uri = Uri.fromFile(file);
        intent.setData(uri);
        TelinkMeshApplication.getInstance().sendBroadcast(intent);*/
    }

    private void setTimestamp() {
        long time = MeshUtils.getTaiTime();
        if (meshStorage != null)
            meshStorage.timestamp = String.format("%020X", time);
    }

    // 另存为
    public File saveAs(File dir, String filename) {
        setTimestamp();
        String jsonData = mGson.toJson(meshStorage);
        return FileSystem.writeString(dir, filename, jsonData);
    }

    // 保存mesh信息至本地json， 主要是设备信息修改
    public void saveFromLocalMesh(Mesh mesh, Context context) {
        if (meshStorage == null) {
            clearAndRecreate(context);
        }

        if (meshStorage.nodes != null) {
            Iterator<MeshStorage.Node> nodeIterator = meshStorage.nodes.iterator();
            MeshStorage.Node node;
            while (nodeIterator.hasNext()) {
                node = nodeIterator.next();
                if (!isProvisionerNode(node)) {
                    nodeIterator.remove();
                }
            }
//            meshStorage.nodes.clear();
        } else {
            meshStorage.nodes = new ArrayList<>();
        }

        if (mesh.devices != null) {
            for (DeviceInfo deviceInfo : mesh.devices) {
                meshStorage.nodes.add(convertDeviceInfoToNode(deviceInfo));
            }
        }
        this.save(context);
    }

    // 将json数据转成mesh数据
    public Mesh toLocalMesh() {
        Mesh mesh = new Mesh();

        // 导入 network key， 只导入第一个key
        final MeshStorage.NetworkKey networkKey = meshStorage.netKeys.get(0);
        mesh.networkKey = Arrays.hexToBytes(networkKey.key);
        mesh.netKeyIndex = networkKey.index;

        final MeshStorage.ApplicationKey appKey = meshStorage.appKeys.get(0);
        mesh.appKey = Arrays.hexToBytes(appKey.key);
        mesh.appKeyIndex = appKey.index;
        final int localAddress = getLocalAddress();
        mesh.localAddress = localAddress;
        /*if (localAddress == -1){

        }*/


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
                if (!isProvisionerNode(node)) {
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
                    mesh.devices.add(deviceInfo);
                }
            }
        }
        return mesh;
    }


    /**
     * 删除再重新生成
     * 默认添加一个 provisioner 和 node， 并通过UUID关联
     *
     * @param context
     */
    public void clearAndRecreate(Context context) {
        meshStorage = new MeshStorage();

        meshStorage.meshUUID = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
//        String timeStr = String.format("%020X", new Date().getTime());
//        meshStorage.timestamp = timeStr;
        setTimestamp();
        createAndInsertProvisioner();

        // 添加默认的netKey
        MeshStorage.NetworkKey netKey = new MeshStorage.NetworkKey();
        netKey.name = "Telink Network Key";
        netKey.index = 0;
        netKey.phase = 0;
        netKey.minSecurity = "high";
        netKey.timestamp = meshStorage.timestamp;
        netKey.key = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
        meshStorage.netKeys = new ArrayList<>();
        meshStorage.netKeys.add(netKey);

        // 添加默认的appKey
        MeshStorage.ApplicationKey appKey = new MeshStorage.ApplicationKey();
        appKey.name = "Telink Application Key";
        appKey.index = 0;
        appKey.key = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
        // 绑定的network key index，即所在network
        appKey.boundNetKey = 0;
        meshStorage.appKeys = new ArrayList<>();
        meshStorage.appKeys.add(appKey);

        meshStorage.groups = new ArrayList<>();
        String[] groupNames = context.getResources().getStringArray(R.array.group_name);
        for (int i = 0; i < 8; i++) {
            MeshStorage.Group group = new MeshStorage.Group();
            group.address = String.format("%04X", i | 0xC000);
            group.name = groupNames[i];
            meshStorage.groups.add(group);
        }
    }

    // 创建provisioner, 并创建对应的node
    private boolean createAndInsertProvisioner() {
        // 创建默认provisioner数据
        MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
//        provisioner.UUID = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "").toUpperCase();
        provisioner.UUID = TelinkMeshApplication.getInstance().getLocalUUID();
        provisioner.provisionerName = "Telink Provisioner";
        // 创建 可分配的设备unicast地址列表 0x0001 -- 0x00FF
        provisioner.allocatedUnicastRange = new ArrayList<>();
        provisioner.allocatedGroupRange = new ArrayList<>();
        provisioner.allocatedSceneRange = new ArrayList<>();

        MeshStorage.Provisioner.AddressRange unicastRange;
        MeshStorage.Provisioner.AddressRange groupRange;
        MeshStorage.Provisioner.SceneRange sceneRange;
        if (meshStorage.provisioners == null) {
            meshStorage.provisioners = new ArrayList<>();
            unicastRange = new MeshStorage.Provisioner.AddressRange("0001", "00FF");
            // 创建 可分配的group地址列表: 0xC001 -- 0xC0FF
            groupRange = new MeshStorage.Provisioner.AddressRange("C000", "C0FF");
            sceneRange = new MeshStorage.Provisioner.SceneRange(0x01, 0x0F);
            // 创建 可分配的场景ID列表: 0x01 -- 0x10 16cnt
//            provisioner.allocatedSceneRange.add();
        } else {
            // 遍历后会用max+1
            int unicastMax = 0;
            int groupMax = 0xC000 - 1;
            int sceneMax = 0x00;

            int unicastHigh;
            int groupHigh;
            int sceneHigh;

            // 遍历已存在的 provisioner 信息， 并获取当前最大的unicast, group, scene
            for (MeshStorage.Provisioner pv : meshStorage.provisioners) {
                for (MeshStorage.Provisioner.AddressRange uni : pv.allocatedUnicastRange) {
                    unicastHigh = Integer.valueOf(uni.highAddress, 16);
                    if (unicastHigh > unicastMax) {
                        unicastMax = unicastHigh;
                    }
                }

                for (MeshStorage.Provisioner.AddressRange gp : pv.allocatedGroupRange) {
                    groupHigh = Integer.valueOf(gp.highAddress, 16);
                    if (groupHigh > groupMax) {
                        groupMax = groupHigh;
                    }
                }

                for (MeshStorage.Provisioner.SceneRange sn : pv.allocatedSceneRange) {
                    sceneHigh = sn.lastScene;
                    if (sceneHigh > sceneMax) {
                        sceneMax = sceneHigh;
                    }
                }
            }


            if (unicastMax > UNICAST_ADDRESS_MAX - 0xFF) {
                TelinkLog.e("unicast address overflow");
                return false;
            }


            /*if (groupMax > GROUP_ADDRESS_MAX - 0xFF) {
                TelinkLog.e("group address overflow");
                return false;
            }

            if (sceneMax > SCENE_ID_MAX - 0x0F) {
                TelinkLog.e("scene id overflow");
                return false;
            }*/

            final int unicastAddressRangeSize = 0xFF;
            final int groupAddressRangeSize = 0xFF;
            final int sceneRangeSize = 0x0F;

            final int unicastStart = unicastMax + 1;
            final int groupStart = groupMax + 1;
            final int sceneStart = sceneMax + 1;

            unicastRange = new MeshStorage.Provisioner.AddressRange(String.format("%04X", unicastStart), String.format("%04X", unicastStart + unicastAddressRangeSize));
//            groupRange = new MeshStorage.Provisioner.AddressRange(String.format("%04X", groupStart), String.format("%04X", groupMax + groupAddressRangeSize));
//            sceneRange = new MeshStorage.Provisioner.SceneRange(sceneStart, sceneStart + sceneRangeSize);
            groupRange = new MeshStorage.Provisioner.AddressRange("C000", "C0FF");
            sceneRange = new MeshStorage.Provisioner.SceneRange(0x01, 0x0F);
        }
        provisioner.allocatedUnicastRange.add(unicastRange);
        provisioner.allocatedGroupRange.add(groupRange);
        provisioner.allocatedSceneRange.add(sceneRange);

        localProvisioner = provisioner;
        meshStorage.provisioners.add(provisioner);


        /**
         * 创建与provisioner对应的node信息
         */
        MeshStorage.Node localNode = new MeshStorage.Node();
        // 关联 provisioner 和 node 信息
        localNode.UUID = provisioner.UUID;
        localNode.unicastAddress = String.format("%04X", allocUnicastAddress());
        TelinkLog.d("alloc address: " + localNode.unicastAddress);
        localNode.name = "Provisioner Node";

        // 添加默认的netKey到node
        localNode.netKeys = new ArrayList<>();
        localNode.netKeys.add(new MeshStorage.NodeKey(0, false));

        // 添加默认的appKey到node
        localNode.appKeys = new ArrayList<>();
        localNode.appKeys.add(new MeshStorage.NodeKey(0, false));

        if (meshStorage.nodes == null) {
            meshStorage.nodes = new ArrayList<>();
        }
        meshStorage.nodes.add(localNode);
        return true;
    }

    /**
     * 从指定的provisioner中申请unicast address
     * 遍历区间， 并获取到首个在区间内可用的mesh地址
     * 删除的中间unicast暂时不复用，且不考虑多个element区间越界问题； 后续能获取到 capability 时， 可以更改申请规则;
     *
     * @return node unicast
     */
    public int allocUnicastAddress() {
        for (MeshStorage.Provisioner.AddressRange addressRange : localProvisioner.allocatedUnicastRange) {
            int tarAdr = getAvailableInRange(addressRange);
            if (tarAdr != -1) {
                return tarAdr;
            }
        }
        return -1;
    }

    private int getAvailableInRange(MeshStorage.Provisioner.AddressRange range) {
        int low = Integer.valueOf(range.lowAddress, 16);
        int high = Integer.valueOf(range.highAddress, 16);
        if (meshStorage.nodes == null || meshStorage.nodes.size() == 0) {
            return low;
        } else {
            int max = -1;
            int nodeMax;
            for (MeshStorage.Node node : meshStorage.nodes) {

                nodeMax = Integer.valueOf(node.unicastAddress, 16);
                if (nodeMax >= low && nodeMax < high) {

                    if (node.elements != null && node.elements.size() != 0) {
                        nodeMax += node.elements.size() - 1;
                    }
                    // max == -1 ||
                    if (max < nodeMax) {
                        max = nodeMax;
                    }
                    if (max >= high) {
                        // 最大值被占用
                        return -1;
                    }
                }
            }
            if (max == -1) {
                return low;
            } else {
                return max + 1;
            }

        }
    }

    // json中的node 信息 转成 jni 对应结构体， 用于网络恢复
    public byte[][] getAllVCNodeInfo() {
        List<byte[]> nodeList = new ArrayList<>();

        for (MeshStorage.Node node : meshStorage.nodes) {
            if (!isProvisionerNode(node)) {
                nodeList.add(convertNodeToNodeInfo(node).toVCNodeInfo());
            }
        }
        if (nodeList.size() == 0) {
            return null;
        }
        return nodeList.toArray(new byte[][]{});
    }

    // mesh中的deviceInfo 转成 json 中的 node
    public MeshStorage.Node convertDeviceInfoToNode(DeviceInfo deviceInfo) {
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
            node.features = new MeshStorage.Features((features & 0b0001) == 0 ? 2 : 1,
                    (features & 0b0010) == 0 ? 2 : 1,
                    (features & 0b0100) == 0 ? 2 : 1,
                    (features & 0b1000) == 0 ? 2 : 1);


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
                            model.bind.add(0);

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
                            model.bind.add(0);
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

        return node;
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
            relaySpt = node.features.relay != 2 ? 0b0001 : 0;
            proxySpt = node.features.proxy != 2 ? 0b0010 : 0;
            friendSpt = node.features.friend != 2 ? 0b0100 : 0;
            lowPowerSpt = node.features.lowPower != 2 ? 0b1000 : 0;
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
    private boolean isProvisionerNode(MeshStorage.Node node) {
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
            if (provisioner.UUID.equals(node.UUID)) {
                return true;
            }
        }
        return false;
    }

    // 获取 provisioner 对应的 node 的unicast address
    private int getLocalAddress() {
        if (meshStorage != null) {
            if (meshStorage.nodes != null) {
                for (MeshStorage.Node node : meshStorage.nodes) {
                    /*if (isProvisionerNode(node)) {
                        return Integer.valueOf(node.unicastAddress, 16);
                    }*/
                    if (node.UUID != null && node.UUID.equals(localProvisioner.UUID)) {
                        return Integer.valueOf(node.unicastAddress, 16);
                    }
                }
            }

        }
        return -1;
    }
}
