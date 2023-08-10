package com.telink.ble.mesh.model.db;

import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OobInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

import io.objectbox.Box;
import io.objectbox.BoxStore;
import io.objectbox.query.Query;

public class MeshInfoService {
    private static MeshInfoService instance = new MeshInfoService();
    private Box<MeshInfo> meshInfoBox;
    private Box<NodeInfo> nodeInfoBox;
    private Box<GroupInfo> groupInfoBox;
    private Box<Scene> sceneBox;
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
        oobInfoQuery = oobInfoBox.query().build();
    }

    public MeshInfo getById(long id) {
        return meshInfoBox.get(id);
    }


    public MeshInfo getByUuid(String meshUUID) {
//        meshInfoBox.query()
//        meshUUID.
        return null;
    }

    /**
     * @return all mesh info in db
     */
    public List<MeshInfo> getAll() {
        return meshInfoQuery.find();
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
