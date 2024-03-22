/********************************************************************************************************
 * @file MeshStorageService.java
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

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Element;
import com.telink.ble.mesh.entity.TransitionTime;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PublishModel;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.db.Scheduler;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

public class MeshStorageService {
    private static MeshStorageService instance = new MeshStorageService();

    public static final String JSON_FILE = "mesh.json";

    public static final byte[] VC_TOOL_CPS = new byte[]{
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
     * target mesh - mesh info parsed from jsonStr
     * current mesh means current used mesh info
     * <br/>
     * three possibilities:
     * <p>
     * 1. target mesh is current mesh: replace the nodes in current mesh by nodes in target mesh , then refresh UI
     * <br/>
     * 2. target mesh is not current mesh, but in the Mesh-List {@link MeshInfoService#getAll()}: replace the nodes in current mesh by nodes in target mesh
     * <br/>
     * 3. target mesh is not current mesh, and not in the Mesh-List {@link MeshInfoService#getAll()}: create new mesh and save in Mesh List
     * <p>
     * import external data
     *
     * @return new mesh info
     */
    public MeshInfo importExternal(String jsonStr, BaseActivity activity) {

        MeshInfo newMesh;
        try {
            newMesh = importFromJson(jsonStr);
        } catch (Exception e) {
            e.printStackTrace();
            activity.toastMsg("import failed : " + e.toString());
            return null;
        }
        if (newMesh == null) {
            activity.toastMsg("import failed");
            return null;
        }

        return newMesh;

        /*MeshInfo localMesh = TelinkMeshApplication.getInstance().getMeshInfo();

        if (localMesh.meshUUID.equalsIgnoreCase(newMesh.meshUUID)) {
            MeshService.getInstance().idle(true);
            TelinkMeshApplication.getInstance().setupMesh(newMesh);
            activity.showTipDialog("Tip",
                    "import mesh success, mesh UUID is the same, back to home page to reconnect",
                    (dialog, which) -> {
                        activity.setResult(Activity.RESULT_OK);
                        activity.finish();
                    });
        } else {
            activity.showConfirmDialog("import mesh success, mesh UUID is different, switch to the new mesh?", (dialog, which) -> {
                MeshService.getInstance().idle(true);
                TelinkMeshApplication.getInstance().setupMesh(newMesh);
                activity.setResult(Activity.RESULT_OK);
                activity.finish();
            });
        }
        return true;*/
    }


    /**
     * @param jsonStr json
     * @return
     * @throws Exception
     */
    public MeshInfo importFromJson(String jsonStr) {

        MeshStorage tempStorage = mGson.fromJson(jsonStr, MeshStorage.class);

        if (!validStorageData(tempStorage)) {
            return null;
        }

        MeshInfo targetMesh = tempStorage.convertToMeshInfo();
        String targetMeshUUID = targetMesh.meshUUID;

        // mesh in db
        MeshInfo localMesh = MeshInfoService.getInstance().getByUuid(targetMeshUUID);

        if (localMesh != null) {
            MeshLogger.d("update local mesh");
//            localMesh.nodes = targetMesh.nodes;
            localMesh.nodes.clear();
            localMesh.nodes.addAll(targetMesh.nodes);

//            localMesh.groups = targetMesh.groups;
            localMesh.groups.clear();
            localMesh.groups.addAll(targetMesh.groups);

//            localMesh.allProvisioners = targetMesh.allProvisioners;
            localMesh.allProvisioners.clear();
            localMesh.allProvisioners.addAll(targetMesh.allProvisioners);

//            localMesh.provisionerNodes = targetMesh.provisionerNodes;
            localMesh.provisionerNodes.clear();
            localMesh.provisionerNodes.addAll(targetMesh.provisionerNodes);

//            localMesh.excludedNodes = targetMesh.excludedNodes;
            localMesh.excludedNodes.clear();
            localMesh.excludedNodes.addAll(targetMesh.excludedNodes);

//            localMesh.extendGroups = targetMesh.extendGroups;
            localMesh.extendGroups.clear();
            localMesh.extendGroups.addAll(targetMesh.extendGroups);

//            localMesh.scenes = targetMesh.scenes;
            localMesh.scenes.clear();
            localMesh.scenes.addAll(targetMesh.scenes);
            localMesh.saveOrUpdate();
            return localMesh;
        }

        MeshLogger.d("create new provisioner");

        // mesh not found in db
        // create new provisioner and provisioner node
        if (!targetMesh.createProvisionerInfo()) {
            MeshLogger.d("create new provisioner error");
            return null;
        }

        targetMesh.ivIndex = 0;
        targetMesh.sequenceNumber = 0;
        MeshInfoService.getInstance().addMeshInfo(targetMesh);
        return targetMesh;
    }


    private boolean validStorageData(MeshStorage meshStorage) {
        return meshStorage != null && meshStorage.provisioners != null && meshStorage.provisioners.size() != 0;
    }


    /**
     * save mesh to json file
     *
     * @return file
     */
    public File exportMeshToJson(File dir, String filename, MeshInfo mesh, List<MeshNetKey> selectedNetKeys) {
        MeshStorage meshStorage = convertMeshToJson(mesh, selectedNetKeys);
        String jsonData = mGson.toJson(meshStorage);
        return FileSystem.writeString(dir, filename, jsonData);
    }

    /**
     * @return json string
     */
    public String meshToJsonString(MeshInfo meshInfo, List<MeshNetKey> selectedNetKeys) {
        MeshStorage meshStorage = convertMeshToJson(meshInfo, selectedNetKeys);
        return mGson.toJson(meshStorage);
    }

    /**
     * convert mesh to MeshStorage
     *
     * @param mesh            target meshInfo
     * @param selectedNetKeys selected network key
     */
    private MeshStorage convertMeshToJson(MeshInfo mesh, List<MeshNetKey> selectedNetKeys) {
        MeshStorage meshStorage = new MeshStorage();
        meshStorage.meshUUID = mesh.meshUUID;
        meshStorage.meshName = mesh.meshName;
        // get current time
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssXXX", Locale.getDefault());
        String formattedDate = sdf.format(new Date());
        MeshLogger.d("time : " + formattedDate);
        meshStorage.timestamp = formattedDate;

        // add all netKey
        meshStorage.netKeys = new ArrayList<>();
        MeshStorage.ApplicationKey appKey;
        meshStorage.appKeys = new ArrayList<>();
        for (MeshNetKey meshNetKey : selectedNetKeys) {
            MeshStorage.NetworkKey netKey = new MeshStorage.NetworkKey();
            netKey.name = meshNetKey.name;
            netKey.index = meshNetKey.index;
            netKey.phase = 0;
            // secure | insecure
            netKey.minSecurity = "secure";
            netKey.timestamp = meshStorage.timestamp;
            netKey.key = Arrays.bytesToHexString(meshNetKey.key, "").toUpperCase();
            meshStorage.netKeys.add(netKey);

            // find bound app keys
            for (MeshAppKey ak : mesh.appKeyList) {
                if (ak.boundNetKeyIndex == meshNetKey.index) {
                    appKey = new MeshStorage.ApplicationKey();
                    appKey.name = ak.name;
                    appKey.index = ak.index;
                    appKey.key = Arrays.bytesToHexString(ak.key, "").toUpperCase();
                    appKey.boundNetKey = ak.boundNetKeyIndex;
                    meshStorage.appKeys.add(appKey);
                    break;
                }
            }
        }

        meshStorage.groups = new ArrayList<>();
        List<GroupInfo> groups = mesh.groups;
        for (int i = 0; i < groups.size(); i++) {
            MeshStorage.Group group = new MeshStorage.Group();
            group.address = String.format("%04X", groups.get(i).address);
            group.name = groups.get(i).name;
            meshStorage.groups.add(group);
        }


        if (SharedPreferenceHelper.isLevelServiceEnable(TelinkMeshApplication.getInstance())) {
            // add extend groups
            groups = mesh.extendGroups;
            for (int i = 0; i < groups.size(); i++) {
                MeshStorage.Group group = new MeshStorage.Group();
                group.address = String.format("%04X", groups.get(i).address);
                group.name = groups.get(i).name;
                meshStorage.groups.add(group);
            }
        }

        meshStorage.provisioners = new ArrayList<>();

        for (Provisioner provisioner : mesh.allProvisioners) {
            meshStorage.provisioners.add(provisioner.convert());
        }

        meshStorage.nodes = new ArrayList<>();
        for (NodeInfo node : mesh.nodes) {
            meshStorage.nodes.add(convertDeviceInfoToNode(node, mesh.getDefaultAppKeyIndex()));
        }

        for (NodeInfo node : mesh.excludedNodes) {
            meshStorage.nodes.add(convertDeviceInfoToNode(node, mesh.getDefaultAppKeyIndex()));
        }

        for (NodeInfo node : mesh.provisionerNodes) {
            meshStorage.nodes.add(convertDeviceInfoToNode(node, mesh.getDefaultAppKeyIndex()));
        }

        /*
         * convert [mesh.scenes] to [meshStorage.scenes]
         */
        meshStorage.scenes = new ArrayList<>();
        MeshStorage.Scene scene;
        for (Scene meshScene : mesh.scenes) {
            scene = new MeshStorage.Scene();
            scene.number = String.format("%04X", meshScene.id);
            scene.name = meshScene.name;
            if (meshScene.addressList != null) {
                scene.addresses = new ArrayList<>();
                scene.addresses.addAll(meshScene.addressList);
            }
            meshStorage.scenes.add(scene);
        }

        return meshStorage;
    }


    /**
     * @param mesh instance
     * @deprecated convert mesh instance to MeshStorage instance, for JSON export
     */
    private MeshStorage meshToJson(MeshInfo mesh, List<MeshNetKey> selectedNetKeys) {
        MeshStorage meshStorage = new MeshStorage();

//        meshStorage.meshUUID = MeshUtils.byteArrayToUuid((MeshUtils.generateRandom(16)));
        meshStorage.meshUUID = mesh.meshUUID;
//        long time = MeshUtils.getTaiTime();

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssXXX", Locale.getDefault());
        String formattedDate = sdf.format(new Date());
        MeshLogger.d("time : " + formattedDate);
        meshStorage.timestamp = formattedDate;

        // add all netKey
        meshStorage.netKeys = new ArrayList<>();
        MeshStorage.ApplicationKey appKey;
        meshStorage.appKeys = new ArrayList<>();
        // for (MeshNetKey meshNetKey : mesh.meshNetKeyList) {
        for (MeshNetKey meshNetKey : selectedNetKeys) {
            MeshStorage.NetworkKey netKey = new MeshStorage.NetworkKey();
//            netKey.name = "Telink Network Key";
            netKey.name = meshNetKey.name;
            netKey.index = meshNetKey.index;
            netKey.phase = 0;
            // secure | insecure
            netKey.minSecurity = "secure";
            netKey.timestamp = meshStorage.timestamp;
            netKey.key = Arrays.bytesToHexString(meshNetKey.key, "").toUpperCase();
            meshStorage.netKeys.add(netKey);

            // find bound app keys
            for (MeshAppKey ak : mesh.appKeyList) {
                if (ak.boundNetKeyIndex == meshNetKey.index) {
                    appKey = new MeshStorage.ApplicationKey();
                    appKey.name = ak.name;
                    appKey.index = ak.index;
                    appKey.key = Arrays.bytesToHexString(ak.key, "").toUpperCase();
                    // bound network key index
                    appKey.boundNetKey = ak.boundNetKeyIndex;
                    meshStorage.appKeys.add(appKey);
                    break;
                }
            }

        }


        // add default appKey
        /*MeshStorage.ApplicationKey appKey;
        meshStorage.appKeys = new ArrayList<>();
        for (MeshAppKey ak : mesh.appKeyList) {
            appKey = new MeshStorage.ApplicationKey();
            appKey.name = "Telink Application Key";
            appKey.index = ak.index;
            appKey.key = Arrays.bytesToHexString(ak.key, "").toUpperCase();

            // bound network key index
            appKey.boundNetKey = ak.boundNetKeyIndex;
            meshStorage.appKeys.add(appKey);
        }*/

        meshStorage.groups = new ArrayList<>();
//        String[] groupNames = context.getResources().getStringArray(R.array.group_name);
        List<GroupInfo> groups = mesh.groups;
        for (int i = 0; i < groups.size(); i++) {
            MeshStorage.Group group = new MeshStorage.Group();
            group.address = String.format("%04X", groups.get(i).address);
            group.name = groups.get(i).name;
            meshStorage.groups.add(group);
        }


        if (SharedPreferenceHelper.isLevelServiceEnable(TelinkMeshApplication.getInstance())) {
            // add extend groups
            groups = mesh.extendGroups;
            for (int i = 0; i < groups.size(); i++) {
                MeshStorage.Group group = new MeshStorage.Group();
                group.address = String.format("%04X", groups.get(i).address);
                group.name = groups.get(i).name;
                meshStorage.groups.add(group);
            }
        }

        // create default provisioner
        MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
        provisioner.UUID = mesh.provisionerUUID;
        provisioner.provisionerName = "Telink Provisioner";
        // create uncast range, default: 0x0001 -- 0x00FF
        provisioner.allocatedUnicastRange = new ArrayList<>();


        for (AddressRange range : mesh.unicastRange) {
            provisioner.allocatedUnicastRange.add(
                    new MeshStorage.Provisioner.AddressRange(String.format("%04X", range.low), String.format("%04X", range.high))
            );
        }

        provisioner.allocatedGroupRange = new ArrayList<>();
        provisioner.allocatedGroupRange.add(new MeshStorage.Provisioner.AddressRange("C000", "C0FF"));
        provisioner.allocatedSceneRange = new ArrayList<>();
        provisioner.allocatedSceneRange.add(new MeshStorage.Provisioner.SceneRange(String.format("%04X", 0x01),
                String.format("%04X", 0x0F)));

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

        localNode.deviceKey = MeshStorage.Defaults.LOCAL_DEVICE_KEY;

        localNode.security = MeshSecurity.Secure.getDesc();

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

//        meshStorage.ivIndex = String.format("%08X", mesh.ivIndex);

        /*
         * convert [mesh.scenes] to [meshStorage.scenes]
         */
        meshStorage.scenes = new ArrayList<>();
        if (mesh.scenes != null) {
            MeshStorage.Scene scene;
            for (Scene meshScene : mesh.scenes) {
                scene = new MeshStorage.Scene();
                scene.number = String.format("%04X", meshScene.id);
                scene.name = meshScene.name;
                if (meshScene.addressList != null) {
                    scene.addresses = new ArrayList<>();
                    scene.addresses.addAll(meshScene.addressList);
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


    /**
     * convert meshStorage to mesh instance
     *
     * @param meshStorage imported from json object or web cloud
     * @return mesh
     */
    public boolean updateLocalMesh(MeshStorage meshStorage, MeshInfo mesh) {

        // import all network keys
        boolean isSameMesh = true;
        if (!mesh.meshUUID.equalsIgnoreCase(meshStorage.meshUUID)) {
            isSameMesh = false;
            mesh.meshUUID = meshStorage.meshUUID;
        }
        mesh.meshNetKeyList.clear();// = new ArrayList<>();
        for (MeshStorage.NetworkKey networkKey : meshStorage.netKeys) {
            MeshLogger.d("import netkey : " + networkKey.key);
            mesh.meshNetKeyList.add(
                    new MeshNetKey(networkKey.name, networkKey.index, Arrays.hexToBytes(networkKey.key))
            );
        }

        mesh.appKeyList.clear();// = new ArrayList<>();
        for (MeshStorage.ApplicationKey applicationKey : meshStorage.appKeys) {
            mesh.appKeyList.add(new MeshAppKey(applicationKey.name, applicationKey.index, Arrays.hexToBytes(applicationKey.key), applicationKey.boundNetKey));
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
            if (UUID.fromString(mesh.provisionerUUID).equals(UUID.fromString(provisioner.UUID))) {
                localProvisioner = provisioner;
                maxRangeHigh = -1;
                break;
            } else {
                for (MeshStorage.Provisioner.AddressRange unRange :
                        provisioner.allocatedUnicastRange) {
                    tmpHigh = MeshUtils.hexToIntB(unRange.highAddress);
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

            if (low + 0xFF > MeshUtils.UNICAST_ADDRESS_MAX) {
                MeshLogger.d("no available unicast range");
                return false;
            }
            final int high = low + 0x03FF;
            mesh.unicastRange.clear();// = new ArrayList<AddressRange>();
            mesh.unicastRange.add(new AddressRange(low, high));
            mesh.localAddress = low;
            mesh.resetProvisionIndex(low + 1);
            mesh.addressTopLimit = high;
            mesh.sequenceNumber = 0;
//            MeshStorage.Provisioner.AddressRange unicastRange = localProvisioner.allocatedUnicastRange.get(0);
//
//            mesh.unicastRange = new AddressRange(low, high);

            mesh.ivIndex = MeshInfo.UNINITIALIZED_IVI;
        }


//        mesh.groupRange = new AddressRange(0xC000, 0xC0FF);

//        mesh.sceneRange = new AddressRange(0x01, 0x0F);

        mesh.groups.clear();// = new ArrayList<>();
        mesh.extendGroups.clear();//  = new ArrayList<>();

        if (meshStorage.groups != null) {
            GroupInfo group;
            for (MeshStorage.Group gp : meshStorage.groups) {
                group = new GroupInfo();
                group.name = gp.name;
                group.address = MeshUtils.hexToIntB(gp.address);
                if (group.name.contains("subgroup")) {
                    mesh.extendGroups.add(group);
                } else {
                    mesh.groups.add(group);
                }
            }
        }

        /*if(isSameMesh){
            mesh.nodes = new ArrayList<>();
        }else{

        }*/
        mesh.nodes.clear();

        if (meshStorage.nodes != null) {
            NodeInfo deviceInfo;
            for (MeshStorage.Node node : meshStorage.nodes) {
                if (!isProvisionerNode(meshStorage, node)) {
                    deviceInfo = new NodeInfo();
                    deviceInfo.meshAddress = MeshUtils.hexToIntB(node.unicastAddress);
//                    deviceInfo.deviceUUID =  Arrays.hexToBytes(node.UUID.replace(":", "").replace("-", ""));
                    deviceInfo.deviceUUID = MeshUtils.uuidToByteArray(node.UUID);

                    deviceInfo.elementCnt = node.elements == null ? 0 : node.elements.size();
                    deviceInfo.deviceKey = Arrays.hexToBytes(node.deviceKey);

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
                                        deviceInfo.setPublishModel(publishModel);
                                    }

                                }
                            }
                        }
                    }


                    deviceInfo.subList = subList;
//                    deviceInfo.setPublishModel();
                    deviceInfo.bound = (node.appKeys != null && node.appKeys.size() != 0);

                    deviceInfo.compositionData = convertNodeToNodeInfo(node);

                    if (node.schedulers != null) {
//                        deviceInfo.schedulers = new ArrayList<>();
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

        mesh.scenes.clear();// = new ArrayList<>();
        if (meshStorage.scenes != null && meshStorage.scenes.size() != 0) {
            Scene scene;
            for (MeshStorage.Scene outerScene : meshStorage.scenes) {
                scene = new Scene();
                scene.id = MeshUtils.hexToIntB(outerScene.number);
                scene.name = outerScene.name;
                if (outerScene.addresses != null) {
                    for (String adrInScene : outerScene.addresses) {
                        scene.save(MeshUtils.hexToIntB(adrInScene));
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
        node.UUID = MeshUtils.byteArrayToUuid(deviceInfo.deviceUUID);
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
            node.features = new MeshStorage.Features((features & 0b0001) == 0 ? 2 : 1,
                    (features & 0b0010) == 0 ? 2 : 1,
                    (features & 0b0100) == 0 ? 2 : 1,
                    (features & 0b1000) == 0 ? 2 : 1);
            /*node.features = new MeshStorage.Features(features & 0b0001,
                    features & 0b0010,
                    features & 0b0100,
                    features & 0b1000);*/


            PublishModel publishModel = deviceInfo.getPublishModelTarget();

            if (deviceInfo.compositionData.elements != null) {
                List<Element> elements = deviceInfo.compositionData.elements;
                MeshStorage.Element element;
                for (int i = 0; i < elements.size(); i++) {
                    Element ele = elements.get(i);
                    element = new MeshStorage.Element();
                    element.index = i;
                    element.location = String.format("%04X", ele.location);

                    element.models = new ArrayList<>();
                    MeshStorage.Model model;

                    // find the offset ? (C000 + 1000 + ?)
                    MeshSigModel[] levelAssociatedModels = MeshSigModel.getLevelAssociatedList();
                    int levelModelIndex = -1;
                    for (int j = 0; j < levelAssociatedModels.length; j++) {
                        if (ele.sigModels.contains(MeshSigModel.SIG_MD_G_LEVEL_S.modelId) && ele.sigModels.contains(levelAssociatedModels[j].modelId)) {
                            levelModelIndex = j;
                            break;
                        }
                    }


                    if (ele.sigNum != 0 && ele.sigModels != null) {
                        for (int modelId : ele.sigModels) {
                            model = new MeshStorage.Model();
                            model.modelId = String.format("%04X", modelId);
                            model.bind = new ArrayList<>();
                            model.bind.add(appKeyIndex);

                            model.subscribe = new ArrayList<>();

                            // for default sub models
                            if (inDefaultSubModel(modelId)) {
                                model.subscribe.addAll(deviceInfo.subList);
                            }

                            // for level models:  use the offset
                            if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId && levelModelIndex != -1) {
                                for (String subAdr : deviceInfo.subList) { //
                                    model.subscribe.add(String.format("%04X", GroupInfo.getExtendAddress(MeshUtils.hexToIntB(subAdr), levelModelIndex)));
                                }
                            }


                            if (publishModel != null && publishModel.modelId == modelId) {
                                final MeshStorage.Publish publish = new MeshStorage.Publish();
                                publish.address = String.format("%04X", publishModel.address);
                                publish.index = 0;

                                publish.ttl = publishModel.ttl;
                                TransitionTime transitionTime = TransitionTime.fromTime(publishModel.period);
                                MeshStorage.PublishPeriod period = new MeshStorage.PublishPeriod();
                                period.numberOfSteps = transitionTime.getNumber() & 0xFF;
                                period.resolution = transitionTime.getResolution();
//                                publish.period = publishModel.period;
                                publish.period = period;
                                publish.credentials = publishModel.credential;
                                publish.retransmit = new MeshStorage.Transmit(publishModel.getTransmitCount()
                                        , (publishModel.getTransmitInterval() + 1) * 50);

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
//        node.name = "Common Node";
        node.name = deviceInfo.name;

        // check if appKey list exists to confirm device bound state
        if (deviceInfo.bound) {
            node.appKeys = new ArrayList<>();
            node.appKeys.add(new MeshStorage.NodeKey(appKeyIndex, false));
        }

        node.security = MeshSecurity.Secure.getDesc();

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

        List<Element> elements = compositionData.elements;
        MeshStorage.Element element;
        for (int i = 0; i < elements.size(); i++) {
            Element ele = elements.get(i);
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
            Element infoEle;
            for (MeshStorage.Element element : node.elements) {
                infoEle = new Element();

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


    // check if node is provisioner
    private boolean isProvisionerNode(MeshStorage meshStorage, MeshStorage.Node node) {
        for (MeshStorage.Provisioner provisioner : meshStorage.provisioners) {
            if (UUID.fromString(provisioner.UUID).equals(UUID.fromString(node.UUID))) {
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
