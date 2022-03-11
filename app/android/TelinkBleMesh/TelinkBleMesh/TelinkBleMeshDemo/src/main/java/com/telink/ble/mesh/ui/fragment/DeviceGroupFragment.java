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
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.GroupInfoAdapter;

import java.util.List;

/**
 * Created by kee on 2018/10/10.
 */

public class DeviceGroupFragment extends BaseFragment implements EventListener<String> {

    private GroupInfoAdapter mAdapter;
    private List<GroupInfo> allGroups;
    private Handler delayHandler = new Handler();
    private NodeInfo deviceInfo;
    int address;
    private MeshSigModel[] models = MeshSigModel.getDefaultSubList();
    private int modelIndex = 0;
    private int opGroupAdr;
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
                setDeviceGroupInfo(allGroups.get(position).address
                        , allGroups.get(position).selected ? 1 : 0);
        });

        refreshUI();

        TelinkMeshApplication.getInstance().addEventListener(ModelSubscriptionStatusMessage.class.getName(), this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);

        /*TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SUB_OP_CONFIRM, this);
        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);*/
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    /**
     * @param groupAddress
     * @param type         0 add, 1 delete
     */
    public void setDeviceGroupInfo(int groupAddress, int type) {
        showWaitingDialog("Setting...");
        opGroupAdr = groupAddress;
        modelIndex = 0;
        this.opType = type;
        // on grouping timeout
        delayHandler.postDelayed(this::dismissWaitingDialog, 5 * 1000);
        setNextModel();
    }

    private void setNextModel() {
        if (modelIndex > models.length - 1) {
            if (opType == 0) {
                deviceInfo.subList.add(opGroupAdr);
            } else {
                deviceInfo.subList.remove((Integer) opGroupAdr);
            }
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getActivity());
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
            MeshMessage groupingMessage = ModelSubscriptionSetMessage.getSimple(address, opType, eleAdr, opGroupAdr, models[modelIndex].modelId, true);

            if (!MeshService.getInstance().sendMeshMessage(groupingMessage)) {
                delayHandler.removeCallbacksAndMessages(null);
                toastMsg("setting fail!");
                dismissWaitingDialog();
            }
        }

    }

    private void getLocalDeviceGroupInfo() {
        allGroups = TelinkMeshApplication.getInstance().getMeshInfo().groups;
        if (deviceInfo.subList == null || deviceInfo.subList.size() == 0) {
            for (GroupInfo group : allGroups) {
                group.selected = false;
            }
            return;
        }
        outer:
        for (GroupInfo group : allGroups) {
            group.selected = false;
            for (int groupAdr : deviceInfo.subList) {
                if (groupAdr == group.address) {
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
