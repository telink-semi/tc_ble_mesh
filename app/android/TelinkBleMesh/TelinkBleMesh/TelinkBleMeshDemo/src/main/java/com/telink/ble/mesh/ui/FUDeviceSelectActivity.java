/********************************************************************************************************
 * @file MeshOTAActivity.java
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
package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.FUDeviceSelectAdapter;

import java.util.ArrayList;
import java.util.List;


/**
 * select target devices for firmware-update
 */
public class FUDeviceSelectActivity extends BaseActivity implements View.OnClickListener,
        BaseSelectableListAdapter.SelectStatusChangedListener,
        EventListener<String> {

    public static final String KEY_SELECTED_DEVICES = "update-selected-devices";

    /**
     * view adapter
     */
    private FUDeviceSelectAdapter normalDeviceAdapter, lpnDeviceAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;

    /**
     * updating devices
     */
    private ArrayList<MeshUpdatingDevice> updatingDevices;

    /**
     * UIView
     */
    private CheckBox cb_device, cb_lpn;

    List<NodeInfo> normalDevices, lpnDevices;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_fu_device_select);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Device Select");
        enableBackNav(true);
        initView();
        addEventListeners();
    }

    private void initView() {
        cb_device = findViewById(R.id.cb_device);
        cb_lpn = findViewById(R.id.cb_lpn);

        RecyclerView rv_device = findViewById(R.id.rv_device);
        RecyclerView rv_lpn = findViewById(R.id.rv_lpn);
        rv_device.setLayoutManager(new LinearLayoutManager(this));
        rv_lpn.setLayoutManager(new LinearLayoutManager(this));

        /*
         * local devices
         */
        normalDevices = new ArrayList<>();
        lpnDevices = new ArrayList<>();
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.selected = false;
                if (deviceInfo.isLpn()) {
                    lpnDevices.add(deviceInfo);
                } else {
                    normalDevices.add(deviceInfo);
                }
            }
        }
//        normalDevice = mesh.nodes;

        normalDeviceAdapter = new FUDeviceSelectAdapter(this, normalDevices);
        normalDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(normalDeviceAdapter);
        cb_device.setChecked(normalDeviceAdapter.allSelected());

        lpnDeviceAdapter = new FUDeviceSelectAdapter(this, lpnDevices);
        lpnDeviceAdapter.setStatusChangedListener(this);
        rv_lpn.setAdapter(lpnDeviceAdapter);
        cb_lpn.setChecked(lpnDeviceAdapter.allSelected());


        Button btn_confirm = findViewById(R.id.btn_confirm);
        btn_confirm.setOnClickListener(this);
        cb_device.setOnClickListener(this);
        cb_lpn.setOnClickListener(this);
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.btn_confirm:
                List<NodeInfo> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
                updatingDevices = new ArrayList<>();
                MeshUpdatingDevice device;
                for (NodeInfo node : nodes) {
                    device = new MeshUpdatingDevice();
                    device.meshAddress = (node.meshAddress);
                    device.updatingEleAddress = (node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    device.isSupported = node.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1;
                    device.pidInfo = node.getPidDesc();
                    device.pid = node.compositionData.pid;
                    updatingDevices.add(device);
                }

                Intent intent = new Intent();
                intent.putParcelableArrayListExtra(KEY_SELECTED_DEVICES, updatingDevices);
                setResult(RESULT_OK, intent);
                finish();
                break;

            case R.id.cb_device:
                normalDeviceAdapter.setAll(!normalDeviceAdapter.allSelected());
                break;
        }
    }


    public List<NodeInfo> getSelectedNodes() {
        List<NodeInfo> nodes = null;

        for (NodeInfo deviceInfo : normalDevices) {
            // deviceInfo.getOnOff() != -1 &&
            if (deviceInfo.selected && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
            }
        }

        for (NodeInfo deviceInfo : lpnDevices) {
            // deviceInfo.selected && deviceInfo.getOnOff() != -1 &&
            if (deviceInfo.selected && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
            }
        }
        return nodes;
    }

    @Override
    public void onSelectStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == normalDeviceAdapter) {
            cb_device.setChecked(normalDeviceAdapter.allSelected());
        } else if (adapter == lpnDeviceAdapter) {
            cb_lpn.setChecked(lpnDeviceAdapter.allSelected());
        }
    }

    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {

        final String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    normalDeviceAdapter.notifyDataSetChanged();
                }
            });
        }
    }


}
