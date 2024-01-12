/********************************************************************************************************
 * @file MeshInfoService.java
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
package com.telink.ble.mesh.model.db;

import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshInfo_;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeLcProps;
import com.telink.ble.mesh.model.OobInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

import io.objectbox.Box;
import io.objectbox.BoxStore;
import io.objectbox.query.Query;
import io.objectbox.query.QueryBuilder;

public class MeshInfoService {
    private static MeshInfoService instance = new MeshInfoService();
    private Box<MeshInfo> meshInfoBox;
    private Box<NodeInfo> nodeInfoBox;
    private Box<GroupInfo> groupInfoBox;
    private Box<Scene> sceneBox;
    private Box<NodeLcProps> nodeLcPropsBox;
    private Box<OobInfo> oobInfoBox;
    private Query<MeshInfo> meshInfoQuery;
    private Query<OobInfo> oobInfoQuery;

    private MeshInfoService() {
    }

    public static MeshInfoService getInstance() {
        return instance;
    }

    public void init(BoxStore store) {
        meshInfoBox = store.boxFor(MeshInfo.class);
        meshInfoQuery = meshInfoBox.query().build();
        nodeInfoBox = store.boxFor(NodeInfo.class);
        oobInfoBox = store.boxFor(OobInfo.class);
        sceneBox = store.boxFor(Scene.class);
        nodeLcPropsBox = store.boxFor(NodeLcProps.class);
        oobInfoQuery = oobInfoBox.query().build();
    }

    public MeshInfo getById(long id) {
        return meshInfoBox.get(id);
    }


    public MeshInfo getByUuid(String meshUUID) {
        Query<MeshInfo> query = meshInfoBox.query().equal(MeshInfo_.meshUUID, meshUUID, QueryBuilder.StringOrder.CASE_INSENSITIVE).build();
        return query.findFirst();
    }

    /**
     * @return all mesh info in db
     */
    public List<MeshInfo> getAll() {
        return meshInfoBox.getAll();
    }


    public void updateMeshInfo(MeshInfo meshInfo) {
        meshInfoBox.put(meshInfo);
    }

    /**
     * add new mesh info
     * the id {@link MeshInfo#id} should be 0
     */
    public void addMeshInfo(MeshInfo meshInfo) {
        meshInfoBox.put(meshInfo);
    }

    /**
     * remove mesh info
     */
    public void removeMeshInfo(MeshInfo meshInfo) {
        meshInfoBox.remove(meshInfo);
    }

    public void removeAllMesh() {
        meshInfoBox.removeAll();
    }

    public void updateNodeInfo(NodeInfo node) {
        MeshLogger.d("updateNodeInfo - " + node.id);
        nodeInfoBox.put(node);
    }

    public void updateNodeLcProps(NodeLcProps props) {
        MeshLogger.d("updateNodeLcProps - " + props.id);
        nodeLcPropsBox.put(props);
    }

    public void updateGroupInfo(GroupInfo groupInfo) {
        groupInfoBox.put(groupInfo);
    }

    public List<OobInfo> getOobList() {
        return oobInfoQuery.find();
    }

    public void updateScene(Scene scene) {
        sceneBox.put(scene);
    }

    public void addOobInfo(List<OobInfo> oobInfoList) {
        oobInfoBox.put(oobInfoList);
    }

    public void addOobInfo(OobInfo... oobInfo) {
        oobInfoBox.put(oobInfo);
    }

    public void updateOobInfo(OobInfo oobInfo) {
        oobInfoBox.put(oobInfo);
    }

    public void removeOobInfo(OobInfo oobInfo) {
        oobInfoBox.remove(oobInfo);
    }

    public void clearAllOobInfo() {
        oobInfoBox.removeAll();
    }

    public byte[] getOobByDeviceUUID(byte[] deviceUUID) {
        for (OobInfo pair : getOobList()) {
            if (Arrays.equals(pair.deviceUUID, deviceUUID)) {
                return pair.oob;
            }
        }
        return null;
    }

    public OobInfo getOobById(long id) {
        return oobInfoBox.get(id);
    }

    public void removeScene(Scene scene) {
        sceneBox.remove(scene);
    }

    public void removeNodeInfo(NodeInfo nodeInfo) {
        nodeInfoBox.remove(nodeInfo);
    }

    public void removeNodes(List<NodeInfo> nodes) {
        nodeInfoBox.remove(nodes);
    }
}
