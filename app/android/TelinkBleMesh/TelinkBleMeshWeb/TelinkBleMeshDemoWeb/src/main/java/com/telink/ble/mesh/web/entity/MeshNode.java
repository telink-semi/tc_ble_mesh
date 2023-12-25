package com.telink.ble.mesh.web.entity;

import android.os.Handler;
import android.util.SparseBooleanArray;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Element;
import com.telink.ble.mesh.entity.ModelPublication;
import com.telink.ble.mesh.entity.TransitionTime;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.OfflineCheckTask;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.model.web.MeshNodePublish;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.MeshNodeStatusChangedEvent;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.io.Serializable;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * <p>
 * mesh node(节点)信息， 一般为已组网设备
 * </p>
 *
 * @author kee
 * @since 2022-11-24
 */
public class MeshNode implements Serializable {

    public final static String URL_NODE_UPLOAD_RECORD = TelinkHttpClient.URL_BASE + "mesh-node/uploadRecord";

    public final static String URL_NODE_DELETE_PUBLISH = TelinkHttpClient.URL_BASE + "mesh-node/deletePublish";

    public final static String URL_NODE_SET_PUBLISH = TelinkHttpClient.URL_BASE + "mesh-node/setPublish";


    private static final long serialVersionUID = 1L;

    public Integer id;

    public String name;

    public String deviceUuid;

    public Integer createUserId;

    public Integer address;

    public Integer state;

    public String deviceKey;

    public Integer bindState;

    public Integer productId;

    public Integer versionId;

    public Integer meshProvisionerId;

    public Long createTime;

    public Long updateTime;

    public String description;

    public Integer networkId;

    public MeshProductInfo productInfo;

    public MeshProductVersionInfo versionInfo;

    public List<MeshGroup> subList;

    // device lightness
    public int lum = 100;

    // device temperature
    public int temp = 0;

    /**
     * device on off state
     * 0:off 1:on -1:offline
     */
    private OnlineState onlineState = OnlineState.OFFLINE;

    /**
     * publication
     */
    private MeshNodePublish publishInfo;

    public boolean selected = false;


    public CompositionData compositionData() {
        if (versionInfo == null) return null;
        return versionInfo.getCompositionDataObj();
    }

    private OfflineCheckTask offlineCheckTask = (OfflineCheckTask) () -> {
        onlineState = OnlineState.OFFLINE;
        MeshLogger.log("offline check task running");
        TelinkMeshApplication.getInstance().dispatchEvent(new MeshNodeStatusChangedEvent(TelinkMeshApplication.getInstance(), NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, MeshNode.this));
    };

    public OnlineState getOnlineState() {
        return onlineState;
    }

    public void setOnlineState(OnlineState onlineState) {
        this.onlineState = onlineState;
        uploadStatusToCloud("node on/off status : " + onlineState);
        if (publishInfo != null) {
            resetOfflineCheckTask();
        }
    }

    public boolean isPubSet() {
        return publishInfo != null && publishInfo.getPeriod() != null && publishInfo.getPeriod() > 0;
    }

    public void setPublishInfo(MeshNodePublish publishInfo) {
        this.publishInfo = publishInfo;
    }

    public void resetPublishInfo(ModelPublication publication) {
        if (publication == null) {
            if (this.publishInfo != null && this.publishInfo.getId() != null) {
                deletePublishCloud(this.publishInfo.getId());
            }
            this.publishInfo = null;
        } else {
            this.publishInfo = MeshNodePublish.from(publication);
            this.publishInfo.setNodeId(this.id);
            this.setPublishCloud(this.publishInfo);
        }
        resetOfflineCheckTask();
    }

    private void resetOfflineCheckTask(){

        Handler handler = TelinkMeshApplication.getInstance().getOfflineCheckHandler();
        handler.removeCallbacks(offlineCheckTask);
        if (this.publishInfo != null && this.onlineState != OnlineState.OFFLINE) {
            int period = TransitionTime.getTimeMilliseconds(publishInfo.getPeriod());
            int timeout = period * 3 + 2000;
            if (timeout > 0) {
                handler.postDelayed(offlineCheckTask, timeout);
            }
        }
    }

    private void deletePublishCloud(int publishId) {
        FormBody formBody = new FormBody.Builder()
                .add("publishId", publishId + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_NODE_DELETE_PUBLISH, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("upload record err");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null, false);
                if (responseInfo == null) {
                    MeshLogger.d("upload record data err");
                    return;
                }
                MeshLogger.d("upload record success");
            }
        });
    }

    private void setPublishCloud(MeshNodePublish publish) {
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", id + "")
                .add("elementAddress", publish.getElementAddress() + "")
                .add("publishAddress", publish.getPublishAddress() + "")
                .add("appKeyIndex", publish.getAppKeyIndex() + "")
                .add("period", publish.getPeriod() + "")
                .add("modelId", publish.getModelId() + "")
                .add("isSigModel", publish.getIsSigModel() + "")
                .add("credentialFlag", publish.getCredentialFlag() + "")
                .add("ttl", publish.getTtl() + "")
                .add("retransmitCount", publish.getRetransmitCount() + "")
                .add("retransmitIntervalSteps", publish.getRetransmitIntervalSteps() + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_NODE_SET_PUBLISH, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("upload record err");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null, false);
                if (responseInfo == null) {
                    MeshLogger.d("upload record data err");
                    return;
                }
                MeshLogger.d("upload record success");
            }
        });
    }

    /**
     * get on/off model element info
     * in panel , multi on/off may exist in different element
     *
     * @return element adr list
     */
    public List<Integer> getEleListByModel(int targetModelId) {
        List<Integer> addressList = new ArrayList<>();

        // element address is based on primary address and increase in loop
        int eleAdr = this.address;
        outer:
        for (Element element : compositionData().elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == targetModelId) {
                        addressList.add(eleAdr++);
                        continue outer;
                    }
                }
            }
            eleAdr++;
        }

        return addressList;
    }

    /**
     * @param tarModelId target model id
     * @return element address: -1 err
     */
    public int getTargetEleAdr(int tarModelId) {
        if (compositionData() == null) return -1;
        int eleAdr = this.address;
        for (Element element : compositionData().elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            if (element.vendorModels != null) {
                for (int modelId : element.vendorModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            eleAdr++;
        }
        return -1;
    }

    /**
     * get lum model element
     *
     * @return lum lightness union info
     */
    public SparseBooleanArray getLumEleInfo() {
        int eleAdr = this.address;
        SparseBooleanArray result = new SparseBooleanArray();
        for (Element element : compositionData().elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean lumSupport = false;
                // if contains lightness model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHTNESS_S.modelId) {
                        lumSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (lumSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }

    /**
     * get element with temperature model
     *
     * @return temp & isLevelSupported
     */
    public SparseBooleanArray getTempEleInfo() {
        int eleAdr = this.address;
        SparseBooleanArray result = new SparseBooleanArray();

        for (Element element : compositionData().elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean tempSupport = false;
                // contains temperature model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId) {
                        tempSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (tempSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }


    @Override
    public String toString() {
        return "MeshNode{" +
                "id=" + id +
                ", name=" + name +
                ", deviceUuid=" + deviceUuid +
                ", createUserId=" + createUserId +
                ", address=" + address +
                ", state=" + state +
                ", deviceKey=" + deviceKey +
                ", bindComplete=" + bindState +
                ", productId=" + productId +
                ", meshProvisionerId=" + meshProvisionerId +
                ", createTime=" + createTime +
                ", updateTime=" + updateTime +
                ", description=" + description +
                "}";
    }

    public boolean isBond() {
        return bindState != null && bindState == 1;
    }

    public boolean isLpn() {
        return this.compositionData().lowPowerSupport();
    }


    /**
     * is node offline
     */
    public boolean isOffline() {
        return this.onlineState == OnlineState.OFFLINE;
    }


    /**
     * is node on
     */
    public boolean isOn() {
        return this.onlineState == OnlineState.ON;
    }


    /**
     * is node off
     */
    public boolean isOff() {
        return this.onlineState == OnlineState.OFF;
    }

    public void addGroup(MeshGroup group) {
        if (subList == null) {
            subList = new ArrayList<>();
        }
        subList.add(group);
    }

    public void removeGroup(MeshGroup group) {
        if (subList == null || subList.size() == 0) return;
        Iterator<MeshGroup> groupIterator = subList.iterator();
        MeshGroup g;
        while (groupIterator.hasNext()) {
            g = groupIterator.next();
            if (g.id.equals(group.id)) {
                groupIterator.remove();
            }
        }
    }

    public String getPidDesc() {
        String pidInfo = "";
        if (isBond() && compositionData() != null) {
            return "cid-" +
                    Arrays.bytesToHexString(MeshUtils.integer2Bytes(compositionData().cid, 2, ByteOrder.LITTLE_ENDIAN), "") +
                    " pid-" +
                    Arrays.bytesToHexString(MeshUtils.integer2Bytes(compositionData().pid, 2, ByteOrder.LITTLE_ENDIAN), "");
//            pidInfo = (compositionData.cid == 0x0211 ? String.format("%04X", compositionData.pid)
//                    : "cid-" + String.format("%02X", compositionData.cid));

        } else {
            pidInfo = "(unbound)";
        }
        return pidInfo;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        MeshNode meshNode = (MeshNode) o;
        return Objects.equals(id, meshNode.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }

    public void uploadStatusToCloud(String status) {
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", id + "")
                .add("status", status)
                .build();
        TelinkHttpClient.getInstance().post(URL_NODE_UPLOAD_RECORD, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("upload record err");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null, false);
                if (responseInfo == null) {
                    MeshLogger.d("upload record data err");
                    return;
                }
                MeshLogger.d("upload record success");
            }
        });
    }
}
