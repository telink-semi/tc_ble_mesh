package com.telink.ble.mesh.web.entity;

import android.util.SparseArray;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * 获取网络详情时， 查询多表返回的结果
 *
 * @author kee
 * @since 2022-11-25
 */
public class MeshNetworkDetail {

    private static final String URL_UPDATE_BIND_STATE = TelinkHttpClient.URL_BASE + "mesh-node/updateNodeBindState";

    private static final String URL_UPDATE_IV_INDEX = TelinkHttpClient.URL_BASE + "mesh-network/updateIvIndex";

    private static final String URL_UPDATE_SNO = TelinkHttpClient.URL_BASE + "mesh-network/updateSequenceNumber";

    private final static String URL_DELETE_NODE = TelinkHttpClient.URL_BASE + "mesh-node/delete";

    public Integer id;

    public String name;

    public Long createTime;

    public int ivIndex;

    /**
     * 网路身份
     *
     * @see NetworkRole#role
     */
    public Integer role;

    /**
     * 由用户ID和clientID获取到的provisioner信息
     */
    public MeshProvisioner provisioner;

    /**
     * 创建该网络的用户名称
     */
    public String createUser;

    /**
     * 关联的network key列表
     */
    public List<MeshNetworkKey> networkKeyList;

    /**
     * 关联的application key列表
     */
    public List<MeshApplicationKey> applicationKeyList;

    /**
     * 已配网设备列表
     */
    public List<MeshNode> nodeList;

    public List<MeshGroup> groupList;


    public MeshConfiguration convertToConfiguration() {
        MeshConfiguration meshConfiguration = new MeshConfiguration();
        meshConfiguration.deviceKeyMap = new SparseArray<>();
        if (nodeList != null) {
            for (MeshNode node : nodeList) {
                meshConfiguration.deviceKeyMap.put(node.address, Arrays.hexToBytes(node.deviceKey));
            }
        }

        MeshNetworkKey networkKey = networkKeyList.get(0);
        meshConfiguration.netKeyIndex = networkKey.getIndex();
        meshConfiguration.networkKey = Arrays.hexToBytes(networkKey.getKey());

        meshConfiguration.appKeyMap = new SparseArray<>();
        if (applicationKeyList != null) {
            for (MeshApplicationKey appKey :
                    applicationKeyList) {
                meshConfiguration.appKeyMap.put(appKey.getIndex(), Arrays.hexToBytes(appKey.getKey()));
            }
        }

        meshConfiguration.ivIndex = ivIndex;

        meshConfiguration.sequenceNumber = provisioner.sequenceNumber;

        meshConfiguration.localAddress = provisioner.address;

        return meshConfiguration;
    }

    public MeshNode getDeviceByMeshAddress(int meshAddress) {
        if (this.nodeList == null)
            return null;

        for (MeshNode info : nodeList) {
            if (info.address == meshAddress)
                return info;
        }
        return null;
    }

    public void updateMeshSnoAndIvIndex(int sequenceNumber, int ivIndex) {
        if (this.ivIndex != ivIndex) {
            MeshLogger.d("update iv index - " + ivIndex);
            this.ivIndex = ivIndex;
            this.updateIvIndex(ivIndex);
        }

        if (this.provisioner.sequenceNumber != sequenceNumber) {
            MeshLogger.d("update sequenceNumber - " + sequenceNumber);
            this.provisioner.sequenceNumber = sequenceNumber;
            this.updateSno(sequenceNumber);
        }
    }


    private void updateIvIndex(int ivIndex) {
        FormBody formBody = new FormBody.Builder()
                .add("networkId", TelinkMeshApplication.getInstance().getMeshInfo().id + "")
                .add("ivIndex", ivIndex + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_IV_INDEX, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.d("fail: server response error");
                    return;
                }
                MeshLogger.d("update iv index success  - " + responseInfo.toString());
            }
        });
    }


    private void updateSno(int sno) {
        FormBody formBody = new FormBody.Builder()
                .add("provisionerId", TelinkMeshApplication.getInstance().getMeshInfo().provisioner.id + "")
                .add("sequenceNumber", sno + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_SNO, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.d("fail: server response error");
                    return;
                }
                MeshLogger.d("update sequence number success - " + responseInfo.toString());
            }
        });
    }


    public MeshNetworkKey getDefaultNetKey() {
        return networkKeyList.get(0);
    }

    public int getDefaultAppKeyIndex() {
        if (applicationKeyList.size() == 0) {
            return 0;
        }
        return applicationKeyList.get(0).index;
    }


    public void insertDevice(MeshNode deviceInfo) {
        nodeList.add(deviceInfo);
    }

    public void removeNode(int meshAddress) {
        if (this.nodeList == null || this.nodeList.size() == 0) return;
        MeshNode target = null;
        Iterator<MeshNode> iterator = nodeList.iterator();
        while (iterator.hasNext()) {
            MeshNode deviceInfo = iterator.next();
            if (deviceInfo.address.equals(meshAddress)) {
                target = deviceInfo;
                iterator.remove();
                break;
            }
        }
        if (target == null) {
            MeshLogger.d("remove error target not found");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", target.id + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_DELETE_NODE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.d("fail: server response error");
                    return;
                }
                MeshLogger.d("remove success - " + responseInfo.toString());
            }
        });


    }


    public void removeNode(MeshNode node) {
        if (this.nodeList == null || this.nodeList.size() == 0) return;

        FormBody formBody = new FormBody.Builder()
                .add("nodeId", node.id + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_DELETE_NODE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.d("fail: server response error");
                    return;
                }
                MeshLogger.d("remove success - " + responseInfo.toString());
            }
        });

        Iterator<MeshNode> iterator = nodeList.iterator();
        while (iterator.hasNext()) {
            MeshNode deviceInfo = iterator.next();
            if (deviceInfo.address.equals(node.address)) {
                iterator.remove();
                return;
            }
        }
    }



    public MeshNode getNodeByAddress(int address) {
        if (this.nodeList == null || this.nodeList.size() == 0) return null;
        for (MeshNode node : nodeList) {
            if (node.address.equals(address)) {
                return node;
            }
        }
        return null;
    }

    public MeshNode getNodeById(int nodeId) {
        if (this.nodeList == null || this.nodeList.size() == 0) return null;
        for (MeshNode node : nodeList) {
            if (node.id.equals(nodeId)) {
                return node;
            }
        }
        return null;
    }

    public void saveBindState(int nodeId, int bindState, int vid) {
        MeshNode node = getNodeById(nodeId);
        saveBindState(node, bindState, vid);
    }

    public void saveBindState(MeshNode node, int bindState, int vid) {
        if (node != null) {
            node.bindState = bindState;
        }
        FormBody formBody = new FormBody.Builder()
                .add("id", node.id + "")
                .add("vid", vid + "")
                .add("bindState", bindState + "")
                .build();

        TelinkHttpClient.getInstance().post(URL_UPDATE_BIND_STATE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.d("fail: server response error");
                    return;
                }
                try {
                    MeshNode meshNode = JSON.parseObject(responseInfo.data, MeshNode.class);
                    updateNode(meshNode);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                MeshLogger.d("update node - " + responseInfo.toString());
            }
        });
    }

    private void updateNode(MeshNode meshNode) {
        for (int i = 0; i < nodeList.size(); i++) {
            if (nodeList.get(i).id.equals(meshNode.id)) {
                nodeList.set(i, meshNode);
                break;
            }
        }
    }

    public int getLocalAddress() {
        return provisioner.address;
    }

    /**
     * get all online nodes
     */
    public int getOnlineCountInAll() {
        if (nodeList == null || nodeList.size() == 0) {
            return 0;
        }

        int result = 0;
        for (MeshNode device : nodeList) {
            if (!device.isOffline()) {
                result++;
            }
        }

        return result;
    }

    /**
     * get online nodes count in group
     */
    public int getOnlineCountInGroup(int groupAddress) {
        if (nodeList == null || nodeList.size() == 0) {
            return 0;
        }
        int result = 0;
        for (MeshNode device : nodeList) {
            if (!device.isOffline()) {
                for (MeshGroup group : device.subList) {
                    if (group.address.equals(groupAddress)) {
                        result++;
                        break;
                    }
                }
            }
        }

        return result;
    }

    /**
     * check whether the user has config network permission
     */
    public boolean hasConfigPermission() {
        return role == NetworkRole.CREATOR.role || role == NetworkRole.MEMBER.role;
    }
}
