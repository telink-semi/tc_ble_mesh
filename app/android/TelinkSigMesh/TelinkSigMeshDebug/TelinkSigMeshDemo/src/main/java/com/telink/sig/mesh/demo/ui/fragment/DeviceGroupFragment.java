/********************************************************************************************************
 * @file DeviceGroupFragment.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.demo.ui.fragment;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.model.MeshCommand;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.GroupInfoAdapter;
import com.telink.sig.mesh.demo.ui.widget.ColorPanel;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.Opcode;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.List;

/**
 * 设备分组fragment
 * Created by kee on 2018/10/10.
 */

public class DeviceGroupFragment extends BaseFragment implements EventListener<String> {

    private GroupInfoAdapter mAdapter;
    private List<Group> allGroups;
    private Handler delayHandler = new Handler();
    private DeviceInfo deviceInfo;
    int address;
    private SigMeshModel[] models = SigMeshModel.getDefaultSubList();
    private int modelIndex = 0;
    private int opGroupAdr;
    private int opType;

    private static final String TAG_CMD = "TAG_CMD";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_device_group, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        address = getArguments().getInt("address");
        deviceInfo = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(address);
        getLocalDeviceGroupInfo();

        RecyclerView rv_groups = view.findViewById(R.id.rv_group);
        mAdapter = new GroupInfoAdapter(getActivity(), allGroups);

        rv_groups.setLayoutManager(new LinearLayoutManager(getActivity()));
        rv_groups.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                if (deviceInfo.getOnOff() != -1)
                    setDeviceGroupInfo(allGroups.get(position).address
                            , allGroups.get(position).selected ? 1 : 0);
            }
        });

        refreshUI();
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SUB_OP_CONFIRM, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);
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
        setNextModel();
    }

    private void setNextModel() {
        if (modelIndex > models.length - 1) {
            if (opType == 0) {
                deviceInfo.subList.add(opGroupAdr);
            } else {
                deviceInfo.subList.remove((Integer) opGroupAdr);
            }
            TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getActivity());
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    getLocalDeviceGroupInfo();
                    mAdapter.notifyDataSetChanged();
                    dismissWaitingDialog();
                }
            });

        } else {
            final int eleAdr = deviceInfo.getTargetEleAdr(models[modelIndex].modelId);
            if (eleAdr == -1) {
                modelIndex++;
                setNextModel();
                return;
            }
            // if (!MeshService.getInstance().cfgCmdSubSet(opCode, address, eleAdr, opGroupAdr, models[modelIndex].modelId, true)) {
            if (!MeshService.getInstance().setSubscription(opType, address, eleAdr, opGroupAdr, models[modelIndex].modelId, true, TAG_CMD)) {
                delayHandler.removeCallbacksAndMessages(null);
                toastMsg("setting fail!");
                dismissWaitingDialog();
            }
        }

    }

    private void getLocalDeviceGroupInfo() {
        allGroups = TelinkMeshApplication.getInstance().getMesh().groups;
        if (deviceInfo.subList == null || deviceInfo.subList.size() == 0) {
            for (Group group : allGroups) {
                group.selected = false;
            }
            return;
        }
        outer:
        for (Group group : allGroups) {
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
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAdapter.setEnabled(deviceInfo.getOnOff() != -1);
            }
        });
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(NotificationEvent.EVENT_TYPE_SUB_OP_CONFIRM)) {
//            modelIndex++;
//            setNextModel();

        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY)) {
            refreshUI();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            refreshUI();
        } else if (event.getType().equals(CommandEvent.EVENT_TYPE_CMD_COMPLETE)) {
            MeshCommand meshCommand = ((CommandEvent) event).getMeshCommand();
            if (meshCommand != null) {
                if (TAG_CMD.equals(meshCommand.tag)) {
                    if (meshCommand.rspCnt >= 1) {
                        modelIndex++;
                        setNextModel();
                    } else {
                        getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                toastMsg("set group sub fail");
                                dismissWaitingDialog();
                            }
                        });
                        TelinkLog.e("set group sub error");
                    }
                }
            }


        }
    }
}
