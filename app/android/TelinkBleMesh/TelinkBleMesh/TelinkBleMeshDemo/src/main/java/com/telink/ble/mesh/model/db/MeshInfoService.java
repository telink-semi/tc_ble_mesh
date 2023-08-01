package com.telink.ble.mesh.model.db;

import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
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
    private Query<MeshInfo> meshInfoQuery;

    private MeshInfoService() {
    }

    public static MeshInfoService getInstance() {
        return instance;
    }

    public void init(BoxStore store) {
        meshInfoBox = store.boxFor(MeshInfo.class);
        meshInfoQuery = meshInfoBox.query().build();
        nodeInfoBox = store.boxFor(NodeInfo.class);
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

    public void updateNodeInfo(NodeInfo node) {
        MeshLogger.d("updateNodeInfo - " + node.id);
        nodeInfoBox.put(node);
    }

    public void updateGroupInfo(GroupInfo groupInfo) {
        groupInfoBox.put(groupInfo);
    }

}
