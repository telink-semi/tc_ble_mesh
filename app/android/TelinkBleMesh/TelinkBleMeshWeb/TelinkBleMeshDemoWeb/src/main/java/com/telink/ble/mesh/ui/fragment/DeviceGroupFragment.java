/********************************************************************************************************
 * @file DeviceGroupFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionSetMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.adapter.GroupInfoAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshGroup;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * Created by kee on 2018/10/10.
 */

public class DeviceGroupFragment extends BaseFragment implements EventListener<String> {
    public final static String URL_ADD_NODE_TO_GROUP = TelinkHttpClient.URL_BASE + "mesh-node/addGroup";

    public final static String URL_REMOVE_NODE_FROM_GROUP = TelinkHttpClient.URL_BASE + "mesh-node/deleteGroup";

    private GroupInfoAdapter mAdapter;
    private List<MeshGroup> allGroups;
    private Handler delayHandler = new Handler();
    private MeshNode deviceInfo;
    int address;
    private MeshSigModel[] models = MeshSigModel.getDefaultSubList();
    private int modelIndex = 0;
    // operating group
    private MeshGroup opGroup;
    private int opType;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_device_group, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        address = getArguments().getInt("address");
        deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        getLocalDeviceGroupInfo();

        RecyclerView rv_groups = view.findViewById(R.id.rv_group);
        mAdapter = new GroupInfoAdapter(getActivity(), allGroups);

        rv_groups.setLayoutManager(new LinearLayoutManager(getActivity()));
        rv_groups.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(position -> {
            if (!deviceInfo.isOffline())
                setDeviceGroupInfo(allGroups.get(position)
                        , allGroups.get(position).selected ? 1 : 0);
        });

        refreshUI();

        TelinkMeshApplication.getInstance().addEventListener(ModelSubscriptionStatusMessage.class.getName(), this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    /**
     * @param group
     * @param type  0 add, 1 delete
     */
    public void setDeviceGroupInfo(MeshGroup group, int type) {
        if (!((BaseActivity) getActivity()).validateConfigPermission()) {
            return;
        }
        showWaitingDialog("Setting...");
        opGroup = group;
        modelIndex = 0;
        this.opType = type;
        // on grouping timeout
        delayHandler.postDelayed(this::dismissWaitingDialog, 5 * 1000);
        setNextModel();
    }

    private void setNextModel() {
        if (modelIndex > models.length - 1) {
            if (opType == 0) {
                deviceInfo.addGroup(opGroup);
            } else {
                deviceInfo.removeGroup(opGroup);
            }
            saveGroupInfoToCloud(opType, opGroup);
            getActivity().runOnUiThread(() -> {
                getLocalDeviceGroupInfo();
                mAdapter.notifyDataSetChanged();
                dismissWaitingDialog();
            });

        } else {
            final int eleAdr = deviceInfo.getTargetEleAdr(models[modelIndex].modelId);
            if (eleAdr == -1) {
                modelIndex++;
                setNextModel();
                return;
            }
            MeshMessage groupingMessage = ModelSubscriptionSetMessage.getSimple(address, opType, eleAdr, opGroup.address, models[modelIndex].modelId, true);

            if (!MeshService.getInstance().sendMeshMessage(groupingMessage)) {
                delayHandler.removeCallbacksAndMessages(null);
                toastMsg("setting fail!");
                dismissWaitingDialog();
            }
        }
    }

    private void saveGroupInfoToCloud(int mode, MeshGroup meshGroup) {
        String url = mode == 0 ? URL_ADD_NODE_TO_GROUP : URL_REMOVE_NODE_FROM_GROUP;
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", deviceInfo.id + "")
                .add("groupId", meshGroup.id + "")
                .add("provisionerId", TelinkMeshApplication.getInstance().getMeshInfo().provisioner.id + "")
                .build();
        TelinkHttpClient.getInstance().post(url, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) return;
                MeshLogger.d("save group success - " + responseInfo.toString());
            }
        });
    }

    private void getLocalDeviceGroupInfo() {
        allGroups = TelinkMeshApplication.getInstance().getMeshInfo().groupList;
        if (deviceInfo.subList == null || deviceInfo.subList.size() == 0) {
            for (MeshGroup group : allGroups) {
                group.selected = false;
            }
            return;
        }
        outer:
        for (MeshGroup group : allGroups) {
            group.selected = false;
            for (MeshGroup meshGroup : deviceInfo.subList) {
                if (meshGroup.address.equals(group.address)) {
                    group.selected = true;
                    continue outer;
                }
            }
        }
    }

    private void refreshUI() {
        getActivity().runOnUiThread(() -> mAdapter.setEnabled(
                deviceInfo.isLpn() || !deviceInfo.isOffline()));
    }

    @Override
    public void performed(Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(ModelSubscriptionStatusMessage.class.getName())) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            ModelSubscriptionStatusMessage statusMessage = (ModelSubscriptionStatusMessage) notificationMessage.getStatusMessage();
            if (statusMessage.getStatus() == ConfigStatus.SUCCESS.code) {
                modelIndex++;
                setNextModel();
            } else {
                delayHandler.removeCallbacksAndMessages(null);
                toastMsg("grouping status fail!");
                dismissWaitingDialog();
            }

        } else if (eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            // device online
            refreshUI();
        } else if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            // device offline
            refreshUI();
        }
    }
}
